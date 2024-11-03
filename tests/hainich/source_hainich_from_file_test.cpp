//
// Created by Phillip on 15.03.24.
//

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/framework/parameter_csv_reader.h"
#include "../../src/core/cpp/io/input_hainich.h"
#include "../../src/core/cpp/modules/model.h"
#include "../../src/core/cpp/io/analysis_hainich.h"
#include "../../src/core/cpp/io/time_series.h"

using std::cout;
using std::endl;
using std::string;

TEST(Hainich_tests, Apply_model_from_file) {


    using std::cout;
    using std::endl;
    using std::string;
    string config_filenname = "../tests/hainich/test/input/config.txt";

    Config config;
    config.Create_hainich();
    config.Export(config_filenname);
    config.Read( config_filenname);


    Parameter_CSV_Reader reader(config.parameters_list_file.Get());
    reader.Parse_Full_Files();

    Parameters params = reader.Get_parameter_list()[0];

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
    ASSERT_LT(analysis.Get_Rmse_J(),MAX_RMSE_J);
    std::cout << "Testing if RMSE psi_stem is not nan...";
    ASSERT_LT(analysis.Get_Rmse_psi_stem(),MAX_RMSE_PSI_STEM);


    auto end_clock = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - start_clock);
    std::cout << "Elapsed time: " << ms.count() << " ms\n";
    std::cout << std::string(64, '-') << std::endl;
    std::cout << std::endl;

}
