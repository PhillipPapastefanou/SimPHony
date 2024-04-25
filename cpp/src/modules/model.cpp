#include "model.h"
#include <cmath>
#include <iostream>
#include "root_distribution_model.h"
#include "soil_water_model.h"

void Leaf_Stem_Ground_Implicit_Model::Set_initial_conditions(double psi_leaf_zero, double psi_stem_zero) {
    psi_leaf = psi_leaf_zero;
    psi_stem_ground = psi_stem_zero;
}

int Leaf_Stem_Ground_Implicit_Model::time_index(double elapsed_seconds) {
    return static_cast<int>(elapsed_seconds / params.input_steplen);
}

double Leaf_Stem_Ground_Implicit_Model::d_psi_leaf(double psi_leaf, double psi_stem) {

    // Update beta parameter that rescales stomatal conductance
    // beta = 0 -> stomata closed; beta = 1 -> stomata fully open
    // The intial function was a simple logic model
    //beta_stom_cond = 1.0 / (1.0 + std::exp(-params.d_50_close * (psi_leaf - params.psi_leaf_50_close)));
    // Gompertz function
    beta_stom_cond = std::exp(-1.0 * std::exp(-1.0 *params.d_50_close*(psi_leaf - psi_gomp_50)));

    // Medlyn phoytosynthesis model
    // mol CO2 s-1 m-2
    gs = params.g0 + beta_stom_cond * (1.0 + params.g1 / std::sqrt(ivpd / ipressure)) * ianet / ica;

    // Convert from Mol CO2 to Mol H2O
    gs *= 1.6;

    // Update stem water flow
    J = stem_flow_module->Get_Stem_flow(psi_stem, psi_leaf);

    // Calculate transpiration
    T = gs * params.leaf_area_index * ivpd / ipressure;

    // Return the derivative of the leaf water potential
    return ((J - T)/ params.leaf_hydraulic_capacitance);
}

double Leaf_Stem_Ground_Implicit_Model::d_psi_stem_ground(double psi_leaf, double psi_stem) {

    // The water stem flow could be also updated right here, but this might lead to inconsistent water uptakes/
    // in case it is updated two times.

    G = 0.0;
    for (int s = 0; s < params.soil_depths.size(); ++s) {
        Gi[s] = root_fraction_player[s] * ik_soil[s] * std::sqrt(params.root_area_index) /
                params.PI / params.soil_depths[s] * (ipsi_soil[s] - psi_stem) / params.grav * params.MPaToPa;

        // Avoid water from flowing down from the stem via roots to the soil
        if (Gi[s] < 0.0)
            Gi[s] = 0.0;

        G += Gi[s];
    }

    // Return the derivative of the stem water potential
    return ((G - J) / (params.stem_hydraulic_capacitance_max * params.canopy_height * params.huber_value));
}

Leaf_Stem_Ground_Implicit_Model::Leaf_Stem_Ground_Implicit_Model
(const Parameters &parameters, const Input &input):
params(parameters),
input_module(input),
output(parameters)
{
    // Gompertz function parameter estimates
    psi_gomp_50 = parameters.psi_leaf_50_close;
    psi_gomp_50 += std::log(std::log(2.0)) / parameters.d_50_close;

}

void Leaf_Stem_Ground_Implicit_Model::Set_derived_parameters() {

    Root_distribution_model root_model(params);
    root_fraction_player = root_model.Get_root_fractions();
    soil_layer_depth_acc = root_model.Get_soil_layer_depth_acc();

    std::string water_model_str;
    switch (params.soil_water_type) {

        case Soil_water_module_type::Saxton06:{
            soil_water_module = std::make_unique<Saxton06_Soil_Water>(params, input_module);
            water_model_str = "Saxton06";
            break;
        }
        case Soil_water_module_type::Campbell:{
            soil_water_module = std::make_unique<Campbell_Soil_Water>(params, input_module);
            water_model_str = "Campbell";
            break;
        }
        case Soil_water_module_type::VanGenuchten:{
            soil_water_module = std::make_unique<Van_Gnuchten_Soil_Water>(params, input_module);
            water_model_str = "VanGnuchten";
            break;
        }
        default:{
            std::cout << "Invalid soil water uptake" << std::endl;
            exit(99);
        }
    }
    // Todo Reenable with logging
    // std::cout << "Using " << water_model_str << " soil water model." << std::endl;

    soil_water_module->CalculatePsiAndKs();

    input_k_soil = soil_water_module->Get_ks();
    input_psi_soil = soil_water_module->Get_psi();

    input_anet = input_module.anet;
    input_vpd = input_module.vpd;

    Gi.resize(params.soil_depths.size());

    switch (params.stem_flow_type) {

        case Stem_flow_module_type::Linear:{
            stem_flow_module = std::make_unique<Linear_stem_flow>(params);
            break;
        }
        case Stem_flow_module_type::KirchhoffWeibull:{
            stem_flow_module = std::make_unique<Kirchhoff_Weibull_stem_flow>(params);
            break;
        }
        case Stem_flow_module_type::KirchhoffPiecewiseErf:{
            stem_flow_module = std::make_unique<Kirchhoff_Piecewise_Erf>(params);
            break;
        }
        default:{
            std::cout << "Invalid soil water uptake" << std::endl;
            exit(99);
        }
    }
    stem_flow_module->Init();


    psi_stem_segments.resize(params.n_stem_segments);

    // Calculate minimum leaf water potential
    min_leaf_water_potential = params.psi88_xylem * params.minimum_psi_leaf_multiplier;

    // Initialise solvers
    solver_psi_leaf = std::make_unique<Bisection_psi_leaf>(*this,
                                                           1E-10,
                                                           100,
                                                           min_leaf_water_potential,
                                                           params.max_leaf_water_potential);

    solver_psi_stem_ground = std::make_unique<Bisection_psi_stem_ground>(*this,
                                                                         1E-10,
                                                                         100,
                                                                         min_leaf_water_potential,
                                                                         params.max_leaf_water_potential);
}

void Leaf_Stem_Ground_Implicit_Model::Run(double steplength, DateTime begin, DateTime end) {

    this->dts = steplength;
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

    for (int i = 0; i < nsteps; ++i) {

        // Update forcing drivers
        ica = 415.0;
        ipressure = 1.013 * 100000.0;

        ianet = input_anet[time_index(ts)];
        ivpd = input_vpd[time_index(ts)];

        ipsi_soil = input_psi_soil[time_index(ts)];
        ik_soil = input_k_soil[time_index(ts)];

        // If we have an extremely unrealistic leaf water potential, stop the solver
        // Plant is assumed to be dead
        if (psi_leaf < min_leaf_water_potential  + 0.5){
            psi_leaf = min_leaf_water_potential;

            T = 0;
            J = 0;

            if (params.verbose){
                std::cout << "Psi leaf  " << psi_leaf;
                std::cout << " Psi stem ground  " << psi_stem_ground  << std::endl;

                std::cout <<   "T: " << T;
                std::cout << "  J: " << J;
                std::cout << "  G: " << G << std::endl;
            }

            // Adding variables to up output files
            add_output();

            // Update time step
            ts += dts;

            continue;
        }

        // Normal solver routine
        else{
            // Convert to maximum change per timestep [MPa]
            const double max_psi_leaf_change_per_ts =  params.max_psi_leaf_change_per_hour * dts / params.SEC_IN_HOUR;

            // Calculate leaf water potential boundaries for bisection solver
            double psi_leaf_lower = psi_leaf - max_psi_leaf_change_per_ts;
            double psi_leaf_upper = psi_leaf + max_psi_leaf_change_per_ts;

            psi_leaf_upper = std::max(psi_leaf_upper , params.max_leaf_water_potential);
            psi_leaf = solver_psi_leaf->Solve(psi_leaf_lower, psi_leaf_upper);
        }

        if (params.verbose) {
            std::cout << "Psi leaf  " << psi_leaf;
        }


        // Lower bound for stem water potential is just the leaf water potential
        // Set the lower bound to be much smaller than leaf water potential to avoid numerical instabilities
        double s0 = psi_leaf ;
        double s1 = params.max_stem_water_potential;

        // We only solve the stem water potential if the leaf water potential is inside some ciritical boundaries
        if(psi_leaf >= min_leaf_water_potential * 0.8){
            psi_stem_ground = solver_psi_stem_ground->Solve(s0, s1);
        }

        update_psi_stems();
        stem_flow_module->Update_min_conductivity_fractions();

        if (params.verbose){
            std::cout << " Psi stem ground  " << psi_stem_ground << std::endl;

            std::cout <<   "T: " << T;
            std::cout << "  J: " << J;
            std::cout << "  G: " << G << std::endl;
        }

        // Adding variables to up output files
        add_output();

        // Update time step
        ts += dts;
    }

}


void Leaf_Stem_Ground_Implicit_Model::add_output() {

    output.Add_Timestep(ts);
    output.Add_DateTime(time_start.AddSeconds(ts));

    output.Add_T(T);
    output.Add_J(J);
    output.Add_G(G);

    vector<float> Gi_f(Gi.begin(), Gi.end());
    for (auto& e: Gi_f)
        e *= 1.0;
    output.Add_G_indiv(Gi_f);
    output.Add_psi_leaf(psi_leaf);
    output.Add_psi_stem_ground(psi_stem_ground);

    vector<float> psi_stems_f(psi_stem_segments.begin(), psi_stem_segments.end());
    output.Add_psi_stems_seg(psi_stems_f);

    vector<float> psi_soil_f(ipsi_soil.begin(), ipsi_soil.end());
    output.Add_psi_soil_indiv(psi_soil_f);

    output.Add_gs(gs);
    output.Add_beta(beta_stom_cond);

    vector<float> ks_soil_f(ik_soil.begin(), ik_soil.end());
    for (auto& e: ks_soil_f)
        e *= 1.0;
    output.Add_ks_indiv(ks_soil_f);

    output.Add_anet(ianet);
    output.Add_vpd(ivpd);

    output.Add_steps_psi_leaf(solver_psi_leaf->Get_nsteps_converged());
    output.Add_steps_psi_stem_ground(solver_psi_stem_ground->Get_nsteps_converged());



}



double Leaf_Stem_Ground_Implicit_Model::psi_stem_root(double psi_stem_target) {
    double d_psi_stem_rec = d_psi_stem_ground(psi_leaf, psi_stem_target);
    return psi_stem_ground + d_psi_stem_rec * dts - psi_stem_target;
}

double Leaf_Stem_Ground_Implicit_Model::psi_leaf_root(double psi_leaf_target) {
    double d_psi_leaf_rec = d_psi_leaf(psi_leaf_target, psi_stem_ground);
    return psi_leaf + d_psi_leaf_rec* dts  - psi_leaf_target;
}

const Output& Leaf_Stem_Ground_Implicit_Model::Get_output() {
    return output;
}

double Bisection_psi_leaf::f(double x) {
    return model.psi_leaf_root(x);
}

double Bisection_psi_stem_ground::f(double x) {
    return model.psi_stem_root(x);
}

void Leaf_Stem_Ground_Implicit_Model::update_psi_stems() {

    // Water potential drop per stem segment
    double delta_psi_per_segment = (psi_stem_ground - psi_leaf) / params.n_stem_segments;

    for (int n = 0; n < params.n_stem_segments; ++n) {
        // Calculate lower and upper water potential of each segment
        const double psi_lower_seg = psi_stem_ground - n * delta_psi_per_segment;
        const double psi_upper_seg = psi_stem_ground - (n + 1) * delta_psi_per_segment;

        // Calculate the average water potential between segments [MPa]
        double psi_avg_seg = (psi_lower_seg + psi_upper_seg) / 2.0;
        psi_stem_segments[n] = psi_avg_seg;
    }

}

