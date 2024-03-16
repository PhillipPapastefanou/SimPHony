//
// Created by Phillip on 27.07.23.
//

#include "swiss_single_test.h"
#include <iostream>
#include <string>


#include "../cpp/parameters.h"
#include "../cpp/input_swiss.h"
#include "../cpp/model.h"
#include <chrono>
#include "../cpp/parameter_csv_reader.h"
#include "../cpp/analysis.h"
#include "../cpp/swiss_drought_trees.h"

using std::cout;
using std::endl;
using std::string;

Swiss_Single_Test::Swiss_Single_Test() {

    string theta_file = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Water_Input_type2.csv";
    string forcing_file = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Forcing_Inter.csv";

    Parameter_CSV_Reader reader("/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/Model/Full_Parameter_setup_12.csv");

    std::string path_of_the_trees = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Trees";

    Swiss_Drought_Trees swiss_drought_tress(path_of_the_trees);

    auto start0 = std::chrono::high_resolution_clock::now();

    reader.Parse_Full_Files();

    auto end0 = std::chrono::high_resolution_clock::now();
    auto ms0 = std::chrono::duration_cast<std::chrono::milliseconds>( end0 - start0);
    std::cout << "Elapsed time: " << ms0.count() << " ms\n";


    // Default parameters
    Parameters params;

    params.huber_value  = 1.0/3600.0;
    params.canopy_height  = 20.0;
    params.g0 = 0.005;
    params.g1 = 1.5;
    params.psi_leaf_50_close = -2.1;
    params.d_50_close = 10.0;
    params.leaf_area_index = 4.8;
    params.leaf_hydraulic_capacitance = 1.0;
    params.stem_hydraulic_capacitance_max = 200 * 1000 / 18.0;
    params.k_xylem_sat = 300;

    params.root_area_index = 24;

    params.soil_depths.assign(11, 0.1);

    params.jackson_root_beta = 0.96;
    params.theta_r = 0.0972;
    params.alpha_genucht = 1.0;
    params.n_genucht = 5.0;
    params.neta_genucht = 0.5;

    params.theta_s = 0.43;
    params.camp_b = 4.5;
    params.camp_psi_soil_ref = -4.5E-3;
    params.k_soil_sat = 20.0/100.0/3600 * 0;

    params.psi50_xylem = -10000.0;

    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.3;


    Input_Swiss input(params);
    input.Add_Forcing_File(forcing_file);
    input.Add_Soilwater_File(theta_file);
    input.Read_N_Parse();

    auto start_clock = std::chrono::high_resolution_clock::now();

    Leaf_Stem_Implicit_Model model(params, input);

    model.Set_derived_parameters();

    model.Set_initial_conditions(psi_leaf_init, psi_stem_init);

    // Length model in seconds
    long steplen      = 1800;

//    double timestart    = 30*2*24 * 0.0;
//    double timeend      = 30*2*24 * 213;

    DateTime begin = input.dates.front();
    DateTime end = begin.AddSeconds(86400 * 20);

    model.Run(steplen, begin, end);

    Analysis analysis(&model, swiss_drought_tress);
    analysis.Run();


    std::cout << "Psi leaf  " << model.Get_output().Get_psi_leaf()[959] << "\n";
    std::cout << "Psi stem  " << model.Get_output().Get_psi_stem()[959] << "\n";


    auto end_clock = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - start_clock);
    std::cout << "Elapsed time: " << ms.count() << " ms\n";

}
