//
// Created by Phillip on 14.07.23.
//
#pragma once
#include "output.h"
#include "input_swiss.h"
#include "input.h"
#include "model.h"
#include "parameters.h"
#include <memory>
#include "analysis.h"
#include "swiss_drought_trees.h"
#include "date_time.h"

class Simulation_Single {

public:
    Simulation_Single();

    void Init_input(std::string theta_file,
                    std::string forcing_file,
                    std::string swiss_trees_folder);

    void Init_parameters_default();
    void Init_parameters_filename(string filename, int index);

    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(double stepleng, DateTime timestart, DateTime timeend);

    Output Get_output();
    Analysis* Get_analysis();

private:
    std::string theta_file;
    std::string forcing_file;

    std::unique_ptr<Parameters> parameters;
    std::unique_ptr<Input> input;
    std::unique_ptr<Swiss_Drought_Trees> swiss_trees;

    std::unique_ptr<Leaf_Stem_Implicit_Model> model;
    std::unique_ptr<Analysis> analysis;



};


