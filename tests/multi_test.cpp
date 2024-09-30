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
#include "../src/framework/simulation_multi_hainich.h"

#include "../src/framework/parameter_csv_reader.h"
#include "../src/io/analysis_swiss.h"
#include "../src/io/swiss_drought_trees.h"

Multi_Test::Multi_Test() {


    string forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/hainich/input/Meteo_Hainich_dT30min_forcing_PHS.csv";
    string sap_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/hainich/eval/SAP_Hainich_Fagus-mean_dT30min_prog.csv";
    string psi_stem_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/hainich/eval/stem_water_pot.csv";
    string parameters_list = "/Users/pp/Documents/Repos/plant_hydro_standalone/py/appl/LHS/generator_files/Hainich_parameters_1000.csv";

    auto start0 = std::chrono::high_resolution_clock::now();

    std::vector<int> indexes(2);
    for (int i = 0; i < indexes.size(); ++i) {
        indexes[i] = i;
    }

    indexes[0] = 0;
    indexes[1] = 29;

    Simulation_Multi_Hainich simulationMulti;
    simulationMulti.Init_Full_Parameter_Setups(parameters_list, indexes);
    simulationMulti.Init_input( forcing_file,sap_file,psi_stem_file, 0);
    simulationMulti.Set_water_pot_initials(-1.0, -0.2);

    DateTime timestart =  DateTime("2023-04-01 00:00:00", "%Y-%m-%d %H:%M:%S");
    DateTime timeend   =  DateTime("2023-11-01 00:00:00", "%Y-%m-%d %H:%M:%S");

    simulationMulti.Run(timestart, timeend);

    auto end0 = std::chrono::high_resolution_clock::now();
    auto ms0 = std::chrono::duration_cast<std::chrono::milliseconds>( end0 - start0);
    std::cout << "Elapsed time: " << ms0.count() << " ms\n";

    for (int i = 0; i <simulationMulti.Get_analysis_list().size() ; ++i) {
        std::cout << simulationMulti.Get_analysis_list()[i].Get_Rmse_psi_stem() << " " << simulationMulti.Get_analysis_list()[i].Get_Rmse_J() << std::endl;
    }



//    string theta_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Water_content_MultiSoils.csv";
//    string forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Forcing_Inter.csv";
//    string parameters_list = "/Users/pp/data/temp/siml/SwissParameterList_03_0550000.csv";
//    string tree_folder_path = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Trees";
//
//    auto start0 = std::chrono::high_resolution_clock::now();
//
//    //std::vector<int> indexes;
//
//    //indexes = { 0, 1};
//    //indexes = { 1};
//
//
//    std::vector<int> indexes(10000);
//    for (int i = 0; i < indexes.size(); ++i) {
//        indexes[i] = i;
//    }
//
//    //indexes = { 4};
//
//    Simulation_Multi_Swiss simulationMulti;
//    simulationMulti.Init_Full_Parameter_Setups(parameters_list, indexes);
//    simulationMulti.Init_input(theta_file, forcing_file,tree_folder_path, 0);
//    simulationMulti.Set_water_pot_initials(-1.0, -0.3);
//
//    DateTime timestart = simulationMulti.Get_first_year();
//    DateTime timeend   = simulationMulti.Get_last_year();
//
//    simulationMulti.Run(timestart, timeend);
//
//    auto end0 = std::chrono::high_resolution_clock::now();
//    auto ms0 = std::chrono::duration_cast<std::chrono::milliseconds>( end0 - start0);
//    std::cout << "Elapsed time: " << ms0.count() << " ms\n";



}
