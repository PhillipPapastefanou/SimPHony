
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/io/input_swiss_std_variation.h"
#include "../../src/core/cpp/framework/simulation_multi_swiss.h"
#include "../../src/core/cpp/io/time_series.h"
#include "../../src/core/cpp/io/analysis_swiss.h"
#include "../../src/core/cpp/io/swiss_drought_trees.h"
#include "../../src/core/cpp/framework/parameter_csv_reader.h"

TEST(Swiss_cc_tests, Apply_model_multi_from_file) {

    std::cout << "Testing if RMSE psi_leaf is not nan...";

    using std::cout;
    using std::endl;
    using std::string;

    string forcing_file = "../data/swiss/input/Forcing_Inter.csv";
    string tree_folder_path = "../data/swiss/eval/Trees";
    string theta_file = "../data/swiss/input/swiss_cc_soil_water_with_sd.csv";
    string parameters_list = "../tests/swiss_cc/test/input/parameter_example_2.csv";


    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.2;


    Simulation_Multi_Swiss simulation;
    simulation.Init_Full_Parameter_Setups(parameters_list, std::vector<int>{0,1});
    simulation.Init_input(theta_file, forcing_file,tree_folder_path, 0);


    DateTime begin =  DateTime("2018-05-01 00:00:00", "%Y-%m-%d %H:%M:%S");
    DateTime end   =  DateTime("2018-12-15 00:00:00", "%Y-%m-%d %H:%M:%S");

    simulation.Set_water_pot_initials(psi_leaf_init, psi_stem_init);
    simulation.Run(begin, end);

    cout << "Done!" << endl;

}

