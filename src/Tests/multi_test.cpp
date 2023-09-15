//
// Created by Phillip on 27.07.23.
//

#include "multi_test.h"
#include <iostream>
#include <string>

#include "../cpp/parameters.h"
#include "../cpp/input.h"
#include "../cpp/simulation_multi.h"
#include <chrono>
#include "../cpp/parameter_csv_reader.h"
#include "../cpp/analysis.h"
#include "../cpp/swiss_drought_trees.h"

Multi_Test::Multi_Test() {


    string theta_file = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Water_Input_type2.csv";
    string forcing_file = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Forcing_Inter.csv";


    std::string path_of_the_trees = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Trees";

    auto start0 = std::chrono::high_resolution_clock::now();

    std::string paramter_list = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/Model/Full_Parameter_setup.csv";

    std::vector<int> indexes(1000);
    for (int i = 0; i < indexes.size(); ++i) {
        indexes[i] = i/10;
    }

    Simulation_Multi simulationMulti;

    simulationMulti.Init_input(theta_file,forcing_file,path_of_the_trees, 0);
    simulationMulti.Init_Full_Parameter_Setups(paramter_list, indexes);
    simulationMulti.Set_water_pot_initials(-1.0, -0.3);

    double steplen = 30;
    double timestart = 30 * 2 * 24 * 0.0;
    double timeend = 30 * 2 * 24 * 213;

    simulationMulti.Run(steplen,timestart, timeend);

    auto end0 = std::chrono::high_resolution_clock::now();
    auto ms0 = std::chrono::duration_cast<std::chrono::milliseconds>( end0 - start0);
    std::cout << "Elapsed time: " << ms0.count() << " ms\n";



}
