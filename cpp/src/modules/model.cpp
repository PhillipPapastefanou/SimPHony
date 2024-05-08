#include "model.h"
#include <cmath>
#include <iostream>

#include "soil_water_model.h"
#include "../framework/solver_indiv_eulerimp.h"

void Model::Set_initial_conditions(double psi_leaf_zero, double psi_stem_zero) {

    water_potential_solver->Init_water_potentials(psi_leaf_zero, psi_stem_zero);
}

int Model::time_index(double elapsed_seconds) {
    return static_cast<int>(elapsed_seconds / params.input_steplen);
}

Model::Model
(Parameters &parameters, Input &input):
params(parameters),
input_module(input),
output(parameters)
{
    parameters.Set_derived();
}

void Model::Set_derived_parameters() {

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

    //water_potential_solver= std::make_unique<Solver_RKF>(params);
    water_potential_solver= std::make_unique<Solver_Indiv_Euler_Imp>(params);
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

    for (int i = 0; i < nsteps; ++i) {

        // Update forcing drivers
        ica = 415.0;
        ipressure = 1.013 * 100000.0;

        ianet = input_anet[time_index(ts)];
        ivpd = input_vpd[time_index(ts)];

        ipsi_soil = input_psi_soil[time_index(ts)];
        ik_soil = input_k_soil[time_index(ts)];

        if (params.verbose){
            std::cout << ts/86400.0 << " ";
        }
        water_potential_solver->Update_forcing(ipsi_soil, ik_soil, ianet, ivpd, ica, ipressure);
        water_potential_solver->Update_water_potentials();

        // Adding variables to up output files
        add_output();

        // Update the output of the solvers aswell
        water_potential_solver->Update_output(output);

        // Update time step
        ts += dts;
    }

}


void Model::add_output() {

    output.Add_Timestep(ts);
    output.Add_DateTime(time_start.AddSeconds(ts));

    vector<float> psi_soil_f(ipsi_soil.begin(), ipsi_soil.end());
    output.Add_psi_soil_indiv(psi_soil_f);

    vector<float> ks_soil_f(ik_soil.begin(), ik_soil.end());
    for (auto& e: ks_soil_f)
        e *= 1.0;
    output.Add_ks_indiv(ks_soil_f);
    output.Add_anet(ianet);
    output.Add_vpd(ivpd);
}

const Output &Model::Get_output() {
    return output;
}
