
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/framework/simulation_multi_swiss.h"
#include "../../src/core/cpp/io/time_series.h"
#include "../../src/core/cpp/io/analysis_swiss.h"
#include "../../src/core/cpp/io/swiss_drought_trees.h"
#include "../../src/core/cpp/framework/parameter_csv_reader.h"

TEST(Swiss_cc_tests, Apply_model_multi_from_file) {

    std::cout << "Testing if RMSE psi_leaf is not nan...\n";

    using std::cout;
    using std::endl;
    using std::string;
    namespace fs = std::filesystem;

    string config_filenname = "../tests/swiss_cc/test/input/config_multi.txt";
    if(!fs::exists(config_filenname)){
        Config config;
        config.Create_swiss_cc();
        config.parameters_list_file.value = "../tests/swiss_cc/test/input/parameter_example_2.csv";
        config.Export(config_filenname);
    }


    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.2;

    Simulation_Multi_Swiss simulation(0, false);


    simulation.Read_Config(config_filenname);

    simulation.Init_Full_Parameter_Setups(
            std::vector<int>{0,1});
    simulation.Init_input();


    DateTime begin =  DateTime("2018-05-01 00:00:00", "%Y-%m-%d %H:%M:%S");
    DateTime end   =  DateTime("2018-12-15 00:00:00", "%Y-%m-%d %H:%M:%S");

    simulation.Init_eval(begin,end);

    simulation.Set_water_pot_initials(psi_leaf_init, psi_stem_init);
    simulation.Run(begin, end);

    vector<Analysis_Swiss> analysis = simulation.Get_analysis_list();

    const double MAX_RMSE_PSI_STEM = 10;

    for (int ai = 0; ai < analysis.size(); ++ai) {
        vector<double> rmse = analysis[0].Get_rmse();

        for (double error: rmse) {
            ASSERT_LT(error, MAX_RMSE_PSI_STEM);
        }
    }
    for (int i = 0; i < analysis[0].Get_rmse().size(); ++i) {

        double x = analysis[0].Get_rmse()[i];
        double y = analysis[1].Get_rmse()[i];

        ASSERT_NE(x, y);
    }


    cout << "Done!" << endl;
    std::cout << std::string(64, '-') << std::endl;
    std::cout << std::endl;

}

