//
// Created by Phillip on 31/10/2024.
//

#include "simulation_single.h"
#include "../io/input_swiss_mult_soils.h"
#include "parameter_csv_reader.h"

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
    input = std::make_unique<Input_Swiss_Multi_Soils>(*config);
    input->Read_N_Parse();
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



