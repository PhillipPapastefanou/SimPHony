//
// Created by Phillip on 14.07.23.
//
#pragma once
#include "output.h"
#include "input.h"
#include "model.h"
#include "parameters.h"
#include <memory>

class Simulation {

public:
    Simulation();

    void Init_input(std::string theta_file, std::string forcing_file);
    void Init_parameters_default();
    void Init_parameters_filename(string filename);

    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(double stepleng, double timestart, double timeend);

    Output Get_output();
private:
    std::string theta_file;
    std::string forcing_file;

    std::unique_ptr<Parameters> parameters;
    std::unique_ptr<Input> input;

    std::unique_ptr<Leaf_Stem_Implicit_Model> model;

};


