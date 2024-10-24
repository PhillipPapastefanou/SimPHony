//
// Created by Phillip on 27.07.23.
//
#pragma once
#include "../io/output.h"
#include "../io/input_swiss_std_variation.h"
#include "../io/input_swiss_indiv_variation.h"
#include "parameters.h"
#include <memory>
#include "../io/analysis_swiss.h"
#include "../io/swiss_drought_trees.h"
#include <tuple>
#include <chrono>
#include "date_time.h"

class Simulation_Multi_Swiss {

public:
    Simulation_Multi_Swiss();
    void Init_input(std::string theta_file, std::string forcing_file, std::string swiss_trees_folder, int rank);
    void Init_Full_Parameter_Setups(string filename, std::vector<int> ids);
    void Init_Partial_Parameter_Setups(string root_filename, string partial_parameter_filename, std::vector<int> ids);
    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(DateTime timestart, DateTime timeend);

    std::vector<Analysis_Swiss> Get_analysis_list();

    DateTime Get_first_date();
    DateTime Get_last_date();
private:
    int rank;

    std::string theta_file;
    std::string forcing_file;

    std::unique_ptr<Input> input;
    std::unique_ptr<Swiss_Drought_Trees> swiss_trees;

    std::vector<std::tuple<Parameters, int> > parameter_list;
    std::vector<Analysis_Swiss> analysis_list;

    double init_psi_stem;
    double init_psi_leaf;
};



