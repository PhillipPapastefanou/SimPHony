#include "model.h"
#include <cmath>
#include <iostream>

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

    soil_water_module->CalculatePsiAndKs();
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

    // Initialise assimlation module
    Assimi_Farquar assimilation(params);

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

        assimilation.Solve_Anet_gs(isw_down * 2.0 * 0.2, ica, vpd_kPa, itemp_air,  beta);

        const double gs = assimilation.Get_Gs();

        water_potential_solver->Update_input(ipsi_soil, ik_soil, gs , ivpd, ipressure);
        water_potential_solver->Update_water_potentials(time_current);


        // Adding variables to up output files
        add_output();
        output.Add_anet(assimilation.Get_An());

        // Update_photosythesis the output of the solvers aswell
        water_potential_solver->Update_output(output);

        // Update_photosythesis time step
        ts += dts;
    }

}

void Model::add_output() {

    output.Add_Timestep(ts);
    output.Add_DateTime(time_start.AddSeconds(ts));

    vector<float> psi_soil_f(ipsi_soil.begin(), ipsi_soil.end());
    // Convert hydraulic head to MPa
    for (int i = 0; i < psi_soil_f.size(); ++i) {
        psi_soil_f[i] *= params.constants.HydraulicHeadInMtoMPa;
    }
    output.Add_psi_soil_indiv(psi_soil_f);

    vector<float> ks_soil_f(ik_soil.begin(), ik_soil.end());
    for (auto& e: ks_soil_f)
        e *= 1.0;
    output.Add_ks_indiv(ks_soil_f);

    output.Add_vpd(ivpd);


    //Todo add other forcings
}

const Output& Model::Get_output() const {
    return output;
}

void Model::Clear_output() {
    output.Clear();
}
