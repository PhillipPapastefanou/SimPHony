//
// Created by Phillip on 14.07.23.
//
#pragma once
#include "../io/output.h"
#include "../io/input.h"
#include "../modules/model.h"
#include "parameters.h"
#include <memory>
#include "../io/analysis_swiss.h"
#include "../io/swiss_drought_trees.h"
#include "date_time.h"

class Simulation_Single_Swiss {

public:
    Simulation_Single_Swiss();

    void Init_input(std::string theta_file,
                    std::string forcing_file,
                    std::string swiss_trees_folder);

    void Init_parameters_default();
    void Init_parameters_filename(string filename, int index);
    void Init_parameters(Parameters params);

    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(DateTime timestart, DateTime timeend);

    Output Get_output();
    Analysis_Swiss Get_analysis();

private:
    std::string theta_file;
    std::string forcing_file;

    std::unique_ptr<Parameters> parameters;
    std::unique_ptr<Input> input;
    std::unique_ptr<Swiss_Drought_Trees> swiss_trees;

    std::unique_ptr<Model> model;
    std::unique_ptr<Analysis_Swiss> analysis;

};


