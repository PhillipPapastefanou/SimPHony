
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/io/input_swiss_mult_soils.h"
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
    string config_filenname = "../tests/swiss_cc/test/input/config.txt";

    Config config;
    config.Create_swiss_cc();
    config.Export(config_filenname);
    config.Read( config_filenname);

    Swiss_Drought_Trees swiss_drought_tress(config.swiss_tree_folder_path.value);

    Parameter_CSV_Reader reader(config.parameters_list_file.Get());
    reader.Parse_Full_Files();

    Parameters params = reader.Get_parameter_list()[0];

    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.2;


    //Input_Swiss_NLayersMeanOneStd input;
    Input_Swiss_Multi_Soils input(config);
    input.Read_N_Parse();

    auto start_clock = std::chrono::high_resolution_clock::now();
    Model model(params, input, config);

    model.Set_derived_parameters();
    model.Set_initial_conditions(psi_leaf_init, psi_stem_init);

    DateTime begin =  DateTime("2018-05-01 00:00:00", "%Y-%m-%d %H:%M:%S");
    DateTime end   =  DateTime("2018-12-15 00:00:00", "%Y-%m-%d %H:%M:%S");

    model.Run(begin, end);

    Analysis_Swiss analysis(model, swiss_drought_tress, params);
    analysis.Run();

    std::vector<double> errors = analysis.Get_rmse();

    const double MAX_RMSE_PSI_STEM = 30;

    for (double error: errors) {
        ASSERT_LT(error,MAX_RMSE_PSI_STEM);
    }

    cout << "Done!" << endl;
    std::cout << std::string(64, '-') << std::endl;
    std::cout << std::endl;

}

