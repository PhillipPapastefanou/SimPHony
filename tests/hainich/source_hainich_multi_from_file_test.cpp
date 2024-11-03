//
// Created by Phillip on 15.03.24.
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/framework/parameter_csv_reader.h"
#include "../../src/core/cpp/io/input_hainich.h"
#include "../../src/core/cpp/framework/simulation_multi_hainich.h"
#include "../../src/core/cpp/io/analysis_hainich.h"
#include "../../src/core/cpp/io/time_series.h"

using std::cout;
using std::endl;
using std::string;

TEST(Hainich_tests, Apply_model_multi_from_file) {

    std::cout << "Testing if RMSE psi_leaf is not nan...\n";

    using std::cout;
    using std::endl;
    using std::string;
    namespace fs = std::filesystem;

    string config_filenname = "../tests/hainich/test/input/config_multi.txt";
    if(!fs::exists(config_filenname)){
        Config config;
        config.Create_hainich();
        config.parameters_list_file.value = "../tests/hainich/test/input/parameter_example_2.csv";
        config.Export(config_filenname);
    }

    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.2;


    Simulation_Multi_Hainich simulation(0, false);

    simulation.Read_Config(config_filenname);

    simulation.Init_Full_Parameter_Setups(
            std::vector<int>{0,1});
    simulation.Init_input();

    DateTime begin =  DateTime("2023-04-01 00:00:00", "%Y-%m-%d %H:%M:%S");
    DateTime end   =  DateTime("2023-11-01 00:00:00", "%Y-%m-%d %H:%M:%S");

    simulation.Init_eval(begin,end);

    simulation.Set_water_pot_initials(psi_leaf_init, psi_stem_init);
    simulation.Run(begin, end);



    vector<AnalysisHainich> analysis = simulation.Get_analysis_list();


    const double MAX_RMSE_PSI_STEM = 10;

    for (int ai = 0; ai < analysis.size(); ++ai) {
        double rmse = 0.0;
        rmse =  analysis[ai].Get_Rmse_G();
        ASSERT_LT(rmse, MAX_RMSE_PSI_STEM);

        rmse =  analysis[ai].Get_Rmse_J();
        ASSERT_LT(rmse, MAX_RMSE_PSI_STEM);

        rmse =  analysis[ai].Get_Rmse_psi_stem();
        ASSERT_LT(rmse, MAX_RMSE_PSI_STEM);
    }


    double x,y = 0.0;

    x = analysis[0].Get_Rmse_G();
    y = analysis[1].Get_Rmse_G();
    ASSERT_NE(x, y);


    x = analysis[0].Get_Rmse_J();
    y = analysis[1].Get_Rmse_J();
    ASSERT_NE(x, y);

    x = analysis[0].Get_Rmse_psi_stem();
    y = analysis[1].Get_Rmse_psi_stem();
    ASSERT_NE(x, y);



//    AnalysisHainich analysis(&model, params);
//    analysis.CompareSapwood(sap_data);
//
//    std::cout << "RMSE G " << analysis.Get_Rmse_G() << "\n";
//    std::cout << "RMSE J " << analysis.Get_Rmse_J() << "\n";
//
//    std::cout << "LL G " << analysis.Get_Log_Likelyhood_G() << "\n";
//    std::cout << "LL J " << analysis.Get_Log_Likelyhood_J() << "\n";
//
//    analysis.ComparePsiStem(psi_stem_data);
//
//    std::cout << "RMSE psi_stem " << analysis.Get_Rmse_psi_stem() << "\n";
//    std::cout << "LL psi_stem " << analysis.Get_Log_Likelyhood_psi_stem() << "\n";
//
//    const double MAX_RMSE_J = 1E99;
//    const double MAX_RMSE_PSI_STEM = 1E99;
//
//    std::cout << "Testing if RMSE J is not nan...";
//    ASSERT_LT(analysis.Get_Rmse_J(),MAX_RMSE_J);
//    std::cout << "Testing if RMSE psi_stem is not nan...";
//    ASSERT_LT(analysis.Get_Rmse_psi_stem(),MAX_RMSE_PSI_STEM);


//    auto end_clock = std::chrono::high_resolution_clock::now();
//    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - start_clock);
//    std::cout << "Elapsed time: " << ms.count() << " ms\n";

    std::cout << std::string(64, '-') << std::endl;
    std::cout << std::endl;
}
