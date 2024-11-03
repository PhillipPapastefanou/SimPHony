//
// Created by Phillip on 15.03.24.
//

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/io/input_hainich.h"
#include "../../src/core/cpp/modules/model.h"
#include "../../src/core/cpp/io/analysis_hainich.h"
#include "../../src/core/cpp/io/time_series.h"

TEST(Hainich_tests, Apply_model_direct) {

    using std::cout;
    using std::endl;
    using std::string;

    Config config;
    config.Create_hainich();

    std::cout << "Testing direct application of the hainich inputs";

    // Default parameters
    Parameters params;

    params.canopy_height = 31.0;
    params.huber_value  = 1.0/3000.0;
    params.k_xylem_sat = 5 * 1000 / 18.0;
    params.stem_hydraulic_capacitance_max = 150 * 1000 / 18.0;
    params.leaf_hydraulic_capacitance = 0.01 *1000/18.0;
    params.g_bark = 0.01;
    params.g0 = 0.005;
    params.g1 = 1.5;
    params.leaf_area_index = 4.8;
    params.psi_leaf_50_close = -2.3;
    params.d_50_close = 2.0;
    params.psi50_xylem = -3.5;
    params.psi88_xylem = -5.5;
    params.root_area_index = 4.5;
    params.jackson_root_beta = 0.96;
    params.tree_density = 64.0 / 10000.0;
    params.anet_max = 2.5;
    params.soil_water_type = Soil_water_module_type::VanGenuchten;

    params.soil_layers.resize(3);


    for (Soil_layer& layer: params.soil_layers) {
        layer.k_soil_sat = 1.0 / 100.0 / 86400.0;
//        layer.clay_fraction = 0.6;
//        layer.sand_fraction = 0.025;
//        layer.organic_matter_fraction = 0.005;

        layer.theta_r =  0.05;
        layer.theta_s =  0.48;
        //layer.camp_b =  6.2;
        layer.psi_soil_sat = -0.5 * 1;
        layer.pore_size_ind = 0.6;
    }

    params.soil_layers[0].depth = 0.08;
    params.soil_layers[1].depth = 0.16;
    params.soil_layers[2].depth = 0.32;

    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.2;

    Input_Hainich input(config);
    input.Read_N_Parse();

    TimeSeries sap_data(config.sap_flow_file.Get(), true, ',');
    sap_data.Load("datetime", "%Y-%m-%d %H:%M:%S", {1});

    TimeSeries psi_stem_data(config.psi_stem_file.Get(), true  , ',');
    psi_stem_data.Load("time", "%Y-%m-%d %H:%M:%S", {1});

    auto start_clock = std::chrono::high_resolution_clock::now();
    Model model(params, input, config);

    model.Set_derived_parameters();
    model.Set_initial_conditions(psi_leaf_init, psi_stem_init);

    DateTime begin =  DateTime("2023-04-01 00:00:00", "%Y-%m-%d %H:%M:%S");
    DateTime end   =  DateTime("2023-11-01 00:00:00", "%Y-%m-%d %H:%M:%S");

    sap_data.GenerateModelObsIndexesSameRes(begin, end, params.dts);
    psi_stem_data.GenerateModelObsIndexes(begin, end, params.dts);

    model.Run(begin, end);

    AnalysisHainich analysis(model, params);
    analysis.CompareSapwood(sap_data);

    std::cout << "RMSE G " << analysis.Get_Rmse_G() << "\n";
    std::cout << "RMSE J " << analysis.Get_Rmse_J() << "\n";

    std::cout << "LL G " << analysis.Get_Log_Likelyhood_G() << "\n";
    std::cout << "LL J " << analysis.Get_Log_Likelyhood_J() << "\n";

    analysis.ComparePsiStem(psi_stem_data);

    std::cout << "RMSE psi_stem " << analysis.Get_Rmse_psi_stem() << "\n";
    std::cout << "LL psi_stem " << analysis.Get_Log_Likelyhood_psi_stem() << "\n";

    const double MAX_RMSE_J = 30;
    const double MAX_RMSE_PSI_STEM = 30;

    std::cout << "Testing if RMSE J is not nan...";
    double rmse_j = analysis.Get_Rmse_J();
    ASSERT_LT(rmse_j,MAX_RMSE_J);
    std::cout << "Testing if RMSE psi_stem is not nan...";
    double rmse_psi_stem = analysis.Get_Rmse_psi_stem();
    ASSERT_LT(analysis.Get_Rmse_psi_stem(),MAX_RMSE_PSI_STEM);

    auto end_clock = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - start_clock);
    std::cout << "Elapsed time: " << ms.count() << " ms\n";
    std::cout << std::string(64, '-') << std::endl;
    std::cout << std::endl;
}
