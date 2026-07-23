#include "model.h"
#include <cmath>
#include <iostream>
#include <chrono>

#include "soil_water/soil_water_model.h"
#include "soil_water/saxton.h"
#include "soil_water/campbell.h"
#include "soil_water/van_genuchten.h"
#include "../framework/solver_indiv_eulerimp.h"
#include "phot/assimilation.h"
#include "phot/assimi_farquar.h"

void Model::Set_initial_conditions(double psi_leaf_zero, double psi_stem_zero) {
    water_potential_solver->Init_water_potentials(psi_leaf_zero, psi_stem_zero);
}

int Model::time_index(double elapsed_seconds) {
    return static_cast<int>(elapsed_seconds / params.dts_input);
}

Model::Model
(Parameters &parameters, Input &input, Config& config):
params(parameters),
input_module(input),
output(parameters),
config(config)
{
    parameters.Set_derived();
}

void Model::Set_derived_parameters() {
    Set_derived_parameters_impl(0, -1);
}

void Model::Set_derived_parameters(DateTime begin, DateTime end) {
    // Same index math Run() uses to look up begin_available/time_index,
    // just done ahead of time so the soil-water precalc below only covers
    // the steps Run(begin, end) will actually read.
    const DateTime begin_available = input_module.dates.front();
    const long ts_begin = begin - begin_available;
    const long ts_end = end - begin_available;
    const int soil_start_idx = static_cast<int>(ts_begin / params.dts_input);
    // +1: inclusive of the last step Run()'s loop can land on after its own
    // (separately-rounded) nsteps computation.
    const int soil_end_idx = static_cast<int>(ts_end / params.dts_input) + 1;

    std::cout << "DEBUG Set_derived_parameters(begin,end): dts_input=" << params.dts_input
              << " ts_begin=" << ts_begin << " ts_end=" << ts_end
              << " soil_start_idx=" << soil_start_idx << " soil_end_idx=" << soil_end_idx
              << " dates.size()=" << input_module.dates.size()
              << " theta_per_layer.size()=" << input_module.theta_per_layer.size()
              << std::endl;

    Set_derived_parameters_impl(soil_start_idx, soil_end_idx);
}

void Model::Set_derived_parameters_impl(int soil_start_idx, int soil_end_idx) {

    std::string water_model_str;
    switch (params.soil_water_type) {
        case Soil_water_module_type::Saxton06:{
            soil_water_module = std::make_unique<Saxton06>(params, input_module, config);
            water_model_str = "Saxton06";
            break;
        }
        case Soil_water_module_type::Campbell:{
            soil_water_module = std::make_unique<Campbell>(params, input_module, config);
            water_model_str = "Campbell";
            break;
        }
        case Soil_water_module_type::VanGenuchten:{
            soil_water_module = std::make_unique<Van_Genuchten>(params, input_module, config);
            water_model_str = "VanGnuchten";
            break;
        }
        default:{
            std::cout << "Invalid soil water uptake" << std::endl;
            exit(99);
        }
    }

    soil_water_module->CalculatePsiAndKs(soil_start_idx, soil_end_idx);
    input_k_soil = soil_water_module->Get_ks();
    input_psi_soil = soil_water_module->Get_psi_soil_head();

    input_air_temperature = input_module.temp_air;
    input_sw_down = input_module.sw_rad;
    input_vpd = input_module.vpd;

    water_potential_solver = std::make_unique<Solver_Indiv_Euler_Imp>(params);
    water_potential_solver->Init_solver();
}

void Model::Run(DateTime begin, DateTime end) {

    const double dts = params.dts;
    time_start = begin;
    time_end = end;

    DateTime begin_available =  input_module.dates.front();
    DateTime end_available =  input_module.dates.back();


    if (begin < begin_available){
        std::cout << "Invalid begin date specified";
        exit(99);
    }

    if (end_available < end){
        std::cout << "Invalid end date specified";
        exit(99);
    }

    // Calculate total simulation lenght in seconds
    delta_Ts = time_end - time_start;

    // Calculate acutal number of steps
    nsteps = delta_Ts / dts;

    // Time difference in seconds to t0
    ts = begin - begin_available ;


    std::cout << "AHLLO begin.t=" << begin.t << " end.t=" << end.t
          << " begin_available.t=" << begin_available.t
          << " end_available.t=" << end_available.t
          << " delta_Ts=" << delta_Ts << " nsteps=" << nsteps
          << " ts=" << ts << std::endl;

    // Initialise assimlation module
    Assimi_Farquar assimilation(params);

    // ---- PROFILE_MODEL_RUN: temporary manual timing, remove when done ----
    // Coarse per-phase timing without needing a profiler set up. Prints a
    // summary once at the end of Run(). Delete this block (and the two
    // timed sections below) once you've moved to a real profiler or are
    // done investigating.
    using clock = std::chrono::high_resolution_clock;
    double ns_assim = 0.0;
    double ns_solve = 0.0;
    double ns_output = 0.0;
    // ------------------------------------------------------------------

    for (int i = 0; i < nsteps; ++i) {

        DateTime time_current = time_start.AddSeconds(ts);

        // Update_photosythesis forcing drivers
        ica = 415.0;
        ipressure = 1.013 * 100000.0;

        itemp_air = input_air_temperature[time_index(ts)];
        ivpd = input_vpd[time_index(ts)];
        isw_down = input_sw_down[time_index(ts)];

        ipsi_soil = input_psi_soil[time_index(ts)];
        ik_soil = input_k_soil[time_index(ts)];


        if (params.verbose){
            std::cout << ts/86400.0 << " ";
        }


        double beta = water_potential_solver->Get_beta();

        const double vpd_kPa = ivpd / 1000.0;

        // ---- PROFILE_MODEL_RUN ----
        auto t0 = clock::now();
        assimilation.Solve_Anet_gs(isw_down * 2.0 * 0.2, ica, vpd_kPa, itemp_air,  beta);
        auto t1 = clock::now();
        ns_assim += std::chrono::duration<double, std::nano>(t1 - t0).count();
        // ---------------------------

        const double gs = assimilation.Get_Gs();

        water_potential_solver->Update_input(ipsi_soil, ik_soil, gs , ivpd, ipressure);

        // ---- PROFILE_MODEL_RUN ----
        auto t2 = clock::now();
        water_potential_solver->Update_water_potentials(time_current);
        auto t3 = clock::now();
        ns_solve += std::chrono::duration<double, std::nano>(t3 - t2).count();
        // ---------------------------


        // ---- PROFILE_MODEL_RUN ----
        auto t4 = clock::now();
        // Adding variables to up output files
        add_output();
        output.Add_anet(assimilation.Get_An());

        // Update_photosythesis the output of the solvers aswell
        water_potential_solver->Update_output(output);
        auto t5 = clock::now();
        ns_output += std::chrono::duration<double, std::nano>(t5 - t4).count();
        // ---------------------------

        // Update_photosythesis time step
        ts += dts;
    }

    // ---- PROFILE_MODEL_RUN ----
    std::cout << "[profile] nsteps=" << nsteps
              << " Solve_Anet_gs: total=" << (ns_assim / 1e6) << "ms"
              << " avg=" << (ns_assim / std::max(1, nsteps)) << "ns/step"
              << " | Update_water_potentials: total=" << (ns_solve / 1e6) << "ms"
              << " avg=" << (ns_solve / std::max(1, nsteps)) << "ns/step"
              << " | output_bookkeeping: total=" << (ns_output / 1e6) << "ms"
              << " avg=" << (ns_output / std::max(1, nsteps)) << "ns/step"
              << std::endl;
    // ---------------------------

}

void Model::add_output() {

    output.Add_Timestep(ts);
    output.Add_DateTime(time_start.AddSeconds(ts));

    // Convert hydraulic head to MPa while appending -- Add_psi_soil_indiv
    // applies the scale itself, so no intermediate vector is built here.
    output.Add_psi_soil_indiv(ipsi_soil, params.constants.HydraulicHeadInMtoMPa);
    output.Add_ks_indiv(ik_soil);

    output.Add_vpd(ivpd);


    //Todo add other forcings
}

const Output& Model::Get_output() const {
    return output;
}

void Model::Clear_output() {
    output.Clear();
}
