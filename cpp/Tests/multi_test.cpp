//
// Created by Phillip on 27.07.23.
//

#include "multi_test.h"
#include <iostream>
#include <string>
#include <chrono>

#include "../src/framework/parameters.h"
#include "../src/io/input_swiss.h"
#include "../src/framework/simulation_multi_swiss.h"

#include "../src/framework/parameter_csv_reader.h"
#include "../src/io/analysis_swiss.h"
#include "../src/io/swiss_drought_trees.h"

Multi_Test::Multi_Test() {


    string theta_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Water_content_MultiSoils.csv";
    string forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Forcing_Inter.csv";
    string parameters_list = "/Users/pp/data/temp/siml/SwissParameterList_03_0550000.csv";
    string tree_folder_path = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Trees";

    auto start0 = std::chrono::high_resolution_clock::now();

    //std::vector<int> indexes;

    //indexes = { 0, 1};
    //indexes = { 1};


    std::vector<int> indexes(10000);
    for (int i = 0; i < indexes.size(); ++i) {
        indexes[i] = i;
    }

    //indexes = { 4};

    Simulation_Multi_Swiss simulationMulti;
    simulationMulti.Init_Full_Parameter_Setups(parameters_list, indexes);
    simulationMulti.Init_input(theta_file, forcing_file,tree_folder_path, 0);
    simulationMulti.Set_water_pot_initials(-1.0, -0.3);

    DateTime timestart = simulationMulti.Get_first_year();
    DateTime timeend   = simulationMulti.Get_last_year();

    simulationMulti.Run(timestart, timeend);

    auto end0 = std::chrono::high_resolution_clock::now();
    auto ms0 = std::chrono::duration_cast<std::chrono::milliseconds>( end0 - start0);
    std::cout << "Elapsed time: " << ms0.count() << " ms\n";



}
