
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/io/input_swiss_std_variation.h"
#include "../../src/core/cpp/io/input_swiss_indiv_variation.h"
#include "../../src/core/cpp/modules/model.h"
#include "../../src/core/cpp/io/time_series.h"
#include "../../src/core/cpp/io/analysis_swiss.h"
#include "../../src/core/cpp/io/swiss_drought_trees.h"
#include "../../src/core/cpp/framework/parameter_csv_reader.h"

TEST(Swiss_cc_tests, Apply_model_from_file) {

    std::cout << "Testing if RMSE psi_leaf is not nan...";

    using std::cout;
    using std::endl;
    using std::string;

    string forcing_file = "../data/swiss/input/Forcing_Inter.csv";
    string tree_folder_path = "../data/swiss/eval/Trees";
    //string theta_file = "../data/swiss/input/swiss_cc_soil_water_with_sd.csv";
    string theta_file = "../data/swiss/input/vwc_swicc_cc_2023_indiv.csv";
    string parameters_list = "../tests/swiss_cc/parameter_example.csv";

    Swiss_Drought_Trees swiss_drought_tress(tree_folder_path);

    Parameter_CSV_Reader reader(parameters_list);
    reader.Parse_Full_Files();

    Parameters params = reader.Get_parameter_list()[0];

    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.2;

    //Input_Swiss_Std_Variation input;
    Input_Swiss_Indiv_Variation input;
    input.Add_Forcing_File(forcing_file);
    input.Add_Soilwater_File(theta_file);
    input.Read_N_Parse();

    auto start_clock = std::chrono::high_resolution_clock::now();
    Model model(params, input);

    model.Set_derived_parameters();
    model.Set_initial_conditions(psi_leaf_init, psi_stem_init);

    DateTime begin =  DateTime("2018-05-01 00:00:00", "%Y-%m-%d %H:%M:%S");
    DateTime end   =  DateTime("2018-12-15 00:00:00", "%Y-%m-%d %H:%M:%S");

    model.Run(begin, end);

    Analysis_Swiss analysis(&model, swiss_drought_tress, params);
    analysis.Run();

    std::vector<double> errors = analysis.Get_rmse();

    const double MAX_RMSE_PSI_STEM = 10;

    for (double error: errors) {
        ASSERT_LT(error,MAX_RMSE_PSI_STEM);
    }

    cout << "Done!" << endl;
    std::cout << std::string(64, '-') << std::endl;
    std::cout << std::endl;

}

