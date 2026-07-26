//
// Tests for the prognostic soil hydrology option (Soil_hydrology_richards): precipitation
// infiltration + vertical Richards-equation transport + root water uptake sink.
//

#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/io/input_hainich.h"
#include "../../src/core/cpp/modules/soil_water/van_genuchten.h"
#include "../../src/core/cpp/modules/soil_water/soil_hydrology_richards.h"
#include <iostream>
#include <numeric>

namespace {

Parameters Make_three_layer_parameters() {
    Parameters parameters;
    parameters.soil_layers.resize(3);

    const double depths[3] = {0.1, 0.3, 0.6};
    for (int i = 0; i < 3; ++i) {
        Soil_layer& layer = parameters.soil_layers[i];
        layer.depth = depths[i];
        layer.k_soil_sat = 9.8084e-06;
        layer.psi_soil_sat = -0.31655;
        layer.theta_s = 0.42685;
        layer.theta_r = 0.033675;
        layer.pore_size_ind = 0.24985;
    }
    return parameters;
}

double Total_storage(const std::vector<double>& theta, const Parameters& parameters) {
    double storage = 0.0;
    for (int i = 0; i < static_cast<int>(theta.size()); ++i) {
        storage += theta[i] * parameters.soil_layers[i].depth;
    }
    return storage;
}

} // namespace

TEST(General_tests, Soil_hydrology_richards_mass_balance)
{
    Parameters parameters = Make_three_layer_parameters();
    Config config;
    Input_Hainich input(config);

    Van_Genuchten curve(parameters, input, config);
    Soil_hydrology_richards richards(parameters, input, config, curve);

    std::vector<float> theta0 = {0.2f, 0.2f, 0.2f};
    richards.Init(theta0);

    std::vector<double> psi0, k0;
    std::vector<float> theta_initial;
    richards.Get_state(psi0, k0, theta_initial);
    std::vector<double> theta_initial_d(theta_initial.begin(), theta_initial.end());
    const double storage_before = Total_storage(theta_initial_d, parameters);

    const double dt = 1800.0;
    const double RHO_WATER = parameters.constants.RHO_WATER;
    const std::vector<double> zero_uptake = {0.0, 0.0, 0.0};

    double net_input_depth = 0.0; // sum((infiltration - drainage)/RHO_WATER * dt)
    const int nsteps = 5;
    for (int s = 0; s < nsteps; ++s) {
        const double precip_rate = 5.0e-5; // kg m-2 s-1, a moderate rain rate
        richards.Step(precip_rate, zero_uptake, dt);

        net_input_depth += (richards.Get_infiltration() - richards.Get_drainage()) / RHO_WATER * dt;

        // Runoff should never be negative, and infiltration should never exceed precip.
        ASSERT_GE(richards.Get_runoff(), 0.0);
        ASSERT_LE(richards.Get_infiltration(), precip_rate + 1.0e-12);
    }

    std::vector<double> psi_end, k_end;
    std::vector<float> theta_end;
    richards.Get_state(psi_end, k_end, theta_end);
    std::vector<double> theta_end_d(theta_end.begin(), theta_end.end());
    const double storage_after = Total_storage(theta_end_d, parameters);

    // Water balance closure: storage change must equal net infiltration minus drainage
    // (no uptake in this test), within a small numerical tolerance.
    ASSERT_NEAR(storage_after - storage_before, net_input_depth, 1.0e-8);

    // All layers must stay within the physical range.
    for (int i = 0; i < 3; ++i) {
        ASSERT_GE(theta_end[i], 0.0);
        ASSERT_LE(theta_end[i], parameters.soil_layers[i].theta_s + 1.0e-9);
    }
}

TEST(General_tests, Soil_hydrology_richards_root_uptake_depletes_storage)
{
    Parameters parameters = Make_three_layer_parameters();
    Config config;
    Input_Hainich input(config);

    Van_Genuchten curve(parameters, input, config);
    Soil_hydrology_richards richards(parameters, input, config, curve);

    std::vector<float> theta0 = {0.3f, 0.3f, 0.3f};
    richards.Init(theta0);

    std::vector<double> psi_prev, k_prev;
    std::vector<float> theta_prev;
    richards.Get_state(psi_prev, k_prev, theta_prev);
    std::vector<double> theta_prev_d(theta_prev.begin(), theta_prev.end());
    double storage_prev = Total_storage(theta_prev_d, parameters);

    const double dt = 1800.0;
    const std::vector<double> uptake = {1.0e-4, 1.0e-4, 1.0e-4}; // mol H2O m-2 s-1 per layer

    for (int s = 0; s < 3; ++s) {
        richards.Step(0.0, uptake, dt);

        std::vector<double> psi_now, k_now;
        std::vector<float> theta_now;
        richards.Get_state(psi_now, k_now, theta_now);
        std::vector<double> theta_now_d(theta_now.begin(), theta_now.end());
        const double storage_now = Total_storage(theta_now_d, parameters);

        // With no precipitation and continuous root uptake, storage must strictly decrease.
        ASSERT_LT(storage_now, storage_prev);

        for (int i = 0; i < 3; ++i) {
            ASSERT_GE(theta_now[i], 0.0);
            ASSERT_LE(theta_now[i], parameters.soil_layers[i].theta_s + 1.0e-9);
        }

        storage_prev = storage_now;
    }
}

TEST(General_tests, Soil_hydrology_richards_max_drainage_conductivity_caps_drainage)
{
    Parameters parameters = Make_three_layer_parameters();
    Config config;
    Input_Hainich input(config);

    // Start near saturation so the bottom layer's own conductivity (and hence its
    // uncapped free-drainage rate) is substantial.
    std::vector<float> theta0 = {0.35f, 0.35f, 0.40f};
    const double dt = 1800.0;
    const double RHO_WATER = parameters.constants.RHO_WATER;
    const std::vector<double> zero_uptake = {0.0, 0.0, 0.0};

    // Uncapped (default) run.
    Van_Genuchten curve_uncapped(parameters, input, config);
    Soil_hydrology_richards richards_uncapped(parameters, input, config, curve_uncapped);
    richards_uncapped.Init(theta0);

    double uncapped_drainage_sum = 0.0;
    for (int s = 0; s < 5; ++s) {
        richards_uncapped.Step(0.0, zero_uptake, dt);
        uncapped_drainage_sum += richards_uncapped.Get_drainage();
    }

    // Capped run: max_drainage_conductivity well below the bottom layer's k_soil_sat.
    Parameters parameters_capped = parameters;
    parameters_capped.max_drainage_conductivity = 1.0e-8;

    Van_Genuchten curve_capped(parameters_capped, input, config);
    Soil_hydrology_richards richards_capped(parameters_capped, input, config, curve_capped);
    richards_capped.Init(theta0);

    const double max_allowed_drainage_kg_m2_s1 = parameters_capped.max_drainage_conductivity * RHO_WATER;
    double capped_drainage_sum = 0.0;
    for (int s = 0; s < 5; ++s) {
        richards_capped.Step(0.0, zero_uptake, dt);
        ASSERT_LE(richards_capped.Get_drainage(), max_allowed_drainage_kg_m2_s1 + 1.0e-12);
        capped_drainage_sum += richards_capped.Get_drainage();
    }

    // The cap must actually bind -- capped drainage should be meaningfully lower than
    // uncapped, not just numerically indistinguishable.
    ASSERT_LT(capped_drainage_sum, uncapped_drainage_sum * 0.5);
}
