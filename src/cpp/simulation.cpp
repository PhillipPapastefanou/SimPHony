//
// Created by Phillip on 14.07.23.
//

#include "simulation.h"
#include "parameter_csv_reader.h"

Simulation::Simulation() {

}

void Simulation::Init_input(std::string theta_file, std::string forcing_file) {

    input = std::make_unique<Input>(theta_file, forcing_file);

    input->Read_N_Parse();

}

void Simulation::Init_parameters_default() {

    parameters = std::make_unique<Parameters>();
}

void Simulation::Set_water_pot_initials(double psi_leaf, double psi_stem) {

    model = std::make_unique<Leaf_Stem_Implicit_Model>(*parameters, *input);

    model->Set_derived_parameters();

    model->Set_initial_conditions(psi_leaf, psi_stem);

}

void Simulation::Run(double steplen, double timestart, double timeend) {

    model->Run(steplen,timestart,timeend);

}

Output Simulation::Get_output() {
    return model->Get_output();
}

void Simulation::Init_parameters_filename(std::string filename) {

    Parameter_CSV_Reader param_reader(filename);

    param_reader.Parse();

    vector<Parameters> list = param_reader.Get_parameter_list();

    parameters = std::make_unique<Parameters>(list[0]);

}

