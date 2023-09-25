//
// Created by Phillip on 14.07.23.
//

#include "simulation_single.h"
#include "parameter_csv_reader.h"

Simulation_Single::Simulation_Single(){

}

void Simulation_Single::Init_input(std::string theta_file, std::string forcing_file, std::string swiss_trees_folder) {

    input = std::make_unique<Input>(theta_file, forcing_file);
    input->Read_N_Parse();

    swiss_trees = std::make_unique<Swiss_Drought_Trees>(swiss_trees_folder);


}

void Simulation_Single::Init_parameters_default() {

    parameters = std::make_unique<Parameters>();
}

void Simulation_Single::Set_water_pot_initials(double psi_leaf, double psi_stem) {

    model = std::make_unique<Leaf_Stem_Implicit_Model>(*parameters, *input);

    model->Set_derived_parameters();

    model->Set_initial_conditions(psi_leaf, psi_stem);

}

void Simulation_Single::Run(double steplen, double timestart, double timeend) {

    model->Run(steplen,timestart,timeend);

    analysis = std::make_unique<Analysis>(model.get(), *swiss_trees);

    analysis->Run();
}

Output Simulation_Single::Get_output() {
    return model->Get_output();
}

void Simulation_Single::Init_parameters_filename(std::string filename, int index) {

    Parameter_CSV_Reader param_reader(filename);

    param_reader.Parse_Full_Files();

    vector<Parameters> list = param_reader.Get_parameter_list();

    parameters = std::make_unique<Parameters>(list[index]);

}

Analysis* Simulation_Single::Get_analysis() {
    return analysis.get();
}
