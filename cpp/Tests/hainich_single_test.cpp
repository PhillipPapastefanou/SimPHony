//
// Created by Phillip on 15.03.24.
//

#include "hainich_single_test.h"
#include <iostream>
#include <string>

#include "../src/framework/parameters.h"
#include "../src/io/input_hainich.h"
#include "../src/modules/model.h"
#include <chrono>
#include "../src/framework/parameter_csv_reader.h"
#include "../src/io/analysis_hainich.h"
#include "../src/io/time_series.h"

using std::cout;
using std::endl;
using std::string;

Hainich_Single_Test::Hainich_Single_Test() {

    string forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/hainich/input/Meteo_Hainich_dT30min_forcing_PHS.csv";
    string sap_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/hainich/eval/SAP_Hainich_Fagus-mean_dT30min_prog.csv";
    string psi_stem_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/hainich/eval/stem_water_pot.csv";

    string parameters_list = "/Users/pp/Documents/Repos/plant_hydro_standalone/py/appl/LHS/generator_files/HainichParameterList_24_1000.csv";


    Parameter_CSV_Reader reader(parameters_list);

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
    params.leaf_hydraulic_capacitance = 0.01 *1000/18.0;
    params.stem_hydraulic_capacitance_max = 100 * 1000 / 18.0;
    params.k_xylem_sat = 10;

    params.root_area_index = 4.5;
    params.jackson_root_beta = 0.96;

    params.soil_water_type = Soil_water_module_type::Campbell;
    params.soil_layers.resize(3);


    for (Soil_layer& layer: params.soil_layers) {
        layer.k_soil_sat = 0.02/100.0/3600;
        layer.clay_fraction = 0.6;
        layer.sand_fraction = 0.025;
        layer.organic_matter_fraction = 0.005;

        layer.theta_r =  0.0972;
        layer.theta_s =  0.52;
        layer.camp_b =  6.2;
        layer.psi_soil_sat = -2.5E-3;
        layer.pore_size_ind = 0.24985;
    }

    params.soil_layers[0].depth = 0.08;
    params.soil_layers[1].depth = 0.16;
    params.soil_layers[2].depth = 0.32;

    params.psi50_xylem = -3.5;
    params.psi88_xylem = -5.5;
    params.tree_density = 100.0 / 10000.0;
    params.sigma_log_likelyhood = 0.01;

    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.3;

    Input_Hainich input(params);
    input.Add_Forcing_File(forcing_file);
    input.Read_N_Parse();


    TimeSeries sap_data(sap_file, true, ',');
    sap_data.Load("datetime", "%Y-%m-%d %H:%M:%S", {1});


    TimeSeries psi_stem_data(psi_stem_file, true  , ',');
    psi_stem_data.Load("time", "%Y-%m-%d %H:%M:%S", {1});


    auto start_clock = std::chrono::high_resolution_clock::now();
    Model model(params, input);

    model.Set_derived_parameters();
    model.Set_initial_conditions(psi_leaf_init, psi_stem_init);

    // Length model in seconds
    long steplen      = 1800;

    DateTime begin = input.dates[0];
    DateTime end = input.dates.back();

    sap_data.GenerateModelObsIndexes(begin, end, params.dts);
    psi_stem_data.GenerateModelObsIndexes(begin, end, params.dts);

    model.Run(begin, end);

    AnalysisHainich analysis(&model, params);
    analysis.CompareSapwood(sap_data);

    std::cout << "RMSE G " << analysis.Get_Rmse_G() << "\n";
    std::cout << "RMSE J " << analysis.Get_Rmse_J() << "\n";

    std::cout << "LL G " << analysis.Get_Log_Likelyhood_G() << "\n";
    std::cout << "LL J " << analysis.Get_Log_Likelyhood_J() << "\n";

    analysis.ComparePsiStem(psi_stem_data);

    std::cout << "RMSE psi_stem " << analysis.Get_Rmse_psi_stem() << "\n";
    std::cout << "LL psi_stem " << analysis.Get_Log_Likelyhood_psi_stem() << "\n";


    auto end_clock = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - start_clock);
    std::cout << "Elapsed time: " << ms.count() << " ms\n";

}
