//
// Created by Phillip on 15.03.24.
//

#include "hainich_single_test.h"
#include <iostream>
#include <string>

#include "../cpp/parameters.h"
#include "../cpp/input_hainich.h"
#include "../cpp/model.h"
#include <chrono>
#include "../cpp/parameter_csv_reader.h"
#include "../cpp/analysis_hainich.h"
#include "../cpp/time_series.h"

using std::cout;
using std::endl;
using std::string;

Hainich_Single_Test::Hainich_Single_Test() {

    string forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/Meteo_Hainich_dT30min_forcing_PHS.csv";
    string sap_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/SAP_Hainich_Fagus-mean_dT30min_prog.csv";


    Parameter_CSV_Reader reader("/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/Model/Full_Parameter_setup_12.csv");

    std::string path_of_the_trees = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Trees";

    //Swiss_Drought_Trees swiss_drought_tress(path_of_the_trees);

    auto start0 = std::chrono::high_resolution_clock::now();

    reader.Parse_Full_Files();

    auto end0 = std::chrono::high_resolution_clock::now();
    auto ms0 = std::chrono::duration_cast<std::chrono::milliseconds>( end0 - start0);
    std::cout << "Elapsed time: " << ms0.count() << " ms\n";


    // Default parameters
    Parameters params;

    params.huber_value  = 1.0/5000.0;
    params.canopy_height  = 35.0;
    params.g0 = 0.005;
    params.g1 = 1.5;
    params.psi_leaf_50_close = -2.3;
    params.d_50_close = 10.0;
    params.leaf_area_index = 4.8;
    params.leaf_hydraulic_capacitance = 1.0;
    params.stem_hydraulic_capacitance_max = 100 * 1000 / 18.0;
    params.k_xylem_sat = 100;

    params.root_area_index = 24;
    params.soil_depths= {0.08, 0.16, 0.32};

    params.jackson_root_beta = 0.96;
    params.theta_r = 0.0972;
    params.alpha_genucht = 1.0;
    params.n_genucht = 5.0;
    params.neta_genucht = 0.5;

    params.theta_s = 0.52;
    params.camp_b = 6.2;
    params.camp_psi_soil_ref = -2.5E-3;
    params.k_soil_sat = 0.02/100.0/3600 ;

    params.psi50_xylem = -3.5;
    params.tree_density = 79.0 / 10000.0;

    params.clay_frac = 0.6;

    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.3;


    Input_Hainich input(params);
    input.Add_Forcing_File(forcing_file);
    input.Read_N_Parse();


    TimeSeries sap_data(sap_file, true, ',');

    sap_data.Load("datetime", "%Y-%m-%d %H:%M:%S", {1});


    auto start_clock = std::chrono::high_resolution_clock::now();

    Leaf_Stem_Implicit_Model model(params, input);

    model.Set_derived_parameters();

    model.Set_initial_conditions(psi_leaf_init, psi_stem_init);

    // Length model in seconds
    long steplen      = 1800;

//    double timestart    = 30*2*24 * 0.0;
//    double timeend      = 30*2*24 * 213;

    DateTime begin = input.dates[1000];
    //DateTime end = begin.AddSeconds(86400 * 20);
    DateTime end = input.dates[10000];


    sap_data.GenerateModelObsIndexes(begin, end, steplen);


    model.Run(steplen, begin, end);

    AnalysisHainich analysis(&model, params);
    analysis.CompareSapwood(sap_data);


    std::cout << "RMSE G " << analysis.Get_Rmse_G() << "\n";
    std::cout << "RMSE J " << analysis.Get_Rmse_J() << "\n";


    auto end_clock = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - start_clock);
    std::cout << "Elapsed time: " << ms.count() << " ms\n";

}
