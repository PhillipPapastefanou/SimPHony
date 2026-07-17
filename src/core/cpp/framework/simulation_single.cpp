//
// Created by Phillip on 31/10/2024.
//

#include "simulation_single.h"
#include "../io/input_swiss_mult_soils.h"
#include "../io/input_hainich.h"
#include "parameter_csv_reader.h"
#include <cmath>

Simulation_Single::Simulation_Single() {

}

void Simulation_Single::Read_Config(std::string config_path) {
    config = std::make_unique<Config>();
    config->Read(config_path);
}

void Simulation_Single::Init_input() {

    if (config == nullptr){
        std::cout << "Config file has not been specified. Did you forget to call the 'Read_Config' function?" << std::endl;
        std::cout << "Exiting...";
        exit(99);
    }

    if (config->location == Location::Swiss_cc)
        input = std::make_unique<Input_Swiss_Multi_Soils>(*config);
    else if (config->location == Location::Hainich)
        input = std::make_unique<Input_Hainich>(*config);

    input->Read_N_Parse();
}
void Simulation_Single::Init_config_web_hainich() {
    config = std::make_unique<Config>();
    config->location = Location::Hainich;
}

void Simulation_Single::Init_input_web() {
    if (config == nullptr){
        std::cout << "Config has not been specified. Call Init_config_web_hainich() first." << std::endl;
        exit(99);
    }
    if (config->location == Location::Swiss_cc)
        input = std::make_unique<Input_Swiss_Multi_Soils>(*config);
    else if (config->location == Location::Hainich)
        input = std::make_unique<Input_Hainich>(*config);
    // Deliberately no input->Read_N_Parse() — the web build supplies forcing
    // data via Set_Forcing_Data_Blob() from a pre-converted binary instead of
    // parsing a CSV off a filesystem that doesn't exist in the browser.
}


void Simulation_Single::Init_soil_layers_default_hainich() {
    if (parameters == nullptr) {
        std::cout << "Parameters have not been specified. Call Init_parameters_default() first." << std::endl;
        exit(99);
    }

    Soil_layer layer{};
    layer.k_soil_sat = 1.0 / 100.0 / 86400.0; // 1 cm/day -> m/s
    layer.psi_soil_sat = -0.5;
    layer.theta_s = 0.48;
    layer.theta_r = 0.05;
    layer.pore_size_ind = 0.6;
    layer.organic_matter_fraction = 0.0;
    layer.sand_fraction = 0.0;
    layer.clay_fraction = 0.0;

    parameters->soil_layers.clear();

    Soil_layer l0 = layer; l0.depth = 0.08; parameters->soil_layers.push_back(l0);
    Soil_layer l1 = layer; l1.depth = 0.16; parameters->soil_layers.push_back(l1);
    Soil_layer l2 = layer; l2.depth = 0.32; parameters->soil_layers.push_back(l2);
}


void Simulation_Single::Set_soil_k_sat_log10(double log10_k_soil_sat) {
    if (parameters == nullptr) {
        std::cout << "Parameters have not been specified. Call Init_parameters_default() first." << std::endl;
        exit(99);
    }
    const double k = std::pow(10.0, log10_k_soil_sat);
    for (auto& layer : parameters->soil_layers) {
        layer.k_soil_sat = k;
    }
}

void Simulation_Single::Init_parameters_default() {
    parameters = std::make_unique<Parameters>();
}

void Simulation_Single::Init_parameters(Parameters params) {
    parameters = std::make_unique<Parameters>(std::move(params));
}

void Simulation_Single::Init_parameters_filename(int index) {

    if (config == nullptr){
        std::cout << "Config file has not been specified. Did you forget to call the 'Read_Config' function?" << std::endl;
        std::cout << "Exiting...";
        exit(99);
    }

    Parameter_CSV_Reader param_reader(config->parameters_list_file.Get());
    param_reader.Parse_Full_Files();
    vector<Parameters> list = param_reader.Get_parameter_list();
    parameters = std::make_unique<Parameters>(list[index]);
}

void Simulation_Single::Set_water_pot_initials(double psi_leaf, double psi_stem) {
    psi_leaf_init = psi_leaf;
    psi_stem_init = psi_stem;
}

void Simulation_Single::Run(DateTime timestart, DateTime timeend) {

    if (config == nullptr){
        std::cout << "Config file has not been specified. Did you forget to call the 'Read_Config' function?" << std::endl;
        std::cout << "Exiting...";
        exit(99);
    }
    model = std::make_unique<Model>(*parameters, *input, *config);
    model->Set_derived_parameters();
    model->Set_initial_conditions(psi_leaf_init, psi_stem_init);
    model->Run(timestart,timeend);
}

void Simulation_Single::Init_eval(DateTime timestart, DateTime timeend) {
    std::cout << "Warning: No evaluation data specified" << std::endl;
}


void Simulation_Single::Analyse() {
    std::cout << "Warning: No analysis specified" << std::endl;
}

Output Simulation_Single::Get_output() {
    return model->Get_output();
}



