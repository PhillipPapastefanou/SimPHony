//
// Created by Phillip on 14.07.23.
//

#include "simulation_single_swiss.h"
#include "parameter_csv_reader.h"
#include "../io/input_swiss.h"
#include "../io/input_swiss_mult_soils.h"
#include "../io/input_swiss_std_variation.h"
#include "../io/input_swiss_indiv_variation.h"

Simulation_Single_Swiss::Simulation_Single_Swiss(){

}

void Simulation_Single_Swiss::Init_input(std::string theta_file, std::string forcing_file, std::string swiss_trees_folder) {

    // Todo: Make the input module also a parameter
    // input = std::make_unique<Input_Swiss_Std_Variation>();
    input = std::make_unique<Input_Swiss_Indiv_Variation>();
    input->Add_Soilwater_File(theta_file);
    input->Add_Forcing_File(forcing_file);
    input->Read_N_Parse();
    swiss_trees = std::make_unique<Swiss_Drought_Trees>(swiss_trees_folder);
}

void Simulation_Single_Swiss::Init_parameters_default() {

    parameters = std::make_unique<Parameters>();
}

void Simulation_Single_Swiss::Set_water_pot_initials(double psi_leaf, double psi_stem) {

    model = std::make_unique<Model>(*parameters, *input);

    model->Set_derived_parameters();

    model->Set_initial_conditions(psi_leaf, psi_stem);

}

void Simulation_Single_Swiss::Run(DateTime timestart, DateTime timeend) {

    model->Run(timestart,timeend);

    analysis = std::make_unique<Analysis_Swiss>(model.get(), *swiss_trees, *parameters);

    analysis->Run();
}

Output Simulation_Single_Swiss::Get_output() {
    return model->Get_output();
}

void Simulation_Single_Swiss::Init_parameters(Parameters params) {
    parameters = std::make_unique<Parameters>(std::move(params));
}


void Simulation_Single_Swiss::Init_parameters_filename(std::string filename, int index) {

    Parameter_CSV_Reader param_reader(filename);

    param_reader.Parse_Full_Files();

    vector<Parameters> list = param_reader.Get_parameter_list();

    parameters = std::make_unique<Parameters>(list[index]);

}

Analysis_Swiss Simulation_Single_Swiss::Get_analysis() {
    return *analysis;
}
