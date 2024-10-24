
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

TEST(Swiss_cc_tests, Apply_model_direct) {

    std::cout << "Testing direct application of the swiss_cc inputs";

    using std::cout;
    using std::endl;
    using std::string;

    string forcing_file = "../data/swiss/input/Forcing_Inter.csv";
    string tree_folder_path = "../data/swiss/eval/Trees";

    Swiss_Drought_Trees swiss_drought_tress(tree_folder_path);

    // Default parameters
    Parameters params;

    params.canopy_height = 31.0;
    params.huber_value  = 1.0/3000.0;
    params.k_xylem_sat = 5 * 1000 / 18.0;
    params.stem_hydraulic_capacitance_max = 150 * 1000 / 18.0 * 0.1;
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


    params.wcont_sigma_deviation = 0.0;

    params.soil_layers.resize(3);

    for (Soil_layer& layer: params.soil_layers) {
        layer.k_soil_sat = 1.0 / 100.0 / 86400.0;

//        layer.clay_fraction = 0.6;
//        layer.sand_fraction = 0.025;
//        layer.organic_matter_fraction = 0.005;
//        layer.camp_b =  6.2;

        layer.theta_r =  0.05;
        layer.theta_s =  0.48;

        layer.psi_soil_sat = -0.5 * 1;
        layer.pore_size_ind = 0.6;
    }

    params.soil_layers[0].depth = 0.1;
    params.soil_layers[1].depth = 0.3;
    params.soil_layers[2].depth = 0.4;


    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.2;


    for (int i = 0; i < 2; ++i) {

        std:unique_ptr<Input> input;
        string theta_file;

        if (i == 0){
            input = std::make_unique<Input_Swiss_Indiv_Variation>();
            theta_file =  "../data/swiss/input/vwc_swicc_cc_2023_indiv.csv";
            std::cout << "Testing the Input_Swiss_Indiv_Variation ...";
        }
        else{
            input = std::make_unique<Input_Swiss_Std_Variation>();
            theta_file = "../data/swiss/input/swiss_cc_soil_water_with_sd.csv";
            std::cout << "Testing the Input_Swiss_Std_Variation ...";
        }

        input->Add_Forcing_File(forcing_file);
        input->Add_Soilwater_File(theta_file);
        input->Read_N_Parse();

        auto start_clock = std::chrono::high_resolution_clock::now();
        Model model(params, *input);

        model.Set_derived_parameters();
        model.Set_initial_conditions(psi_leaf_init, psi_stem_init);

        DateTime begin =  DateTime("2018-05-01 00:00:00", "%Y-%m-%d %H:%M:%S");
        DateTime end   =  DateTime("2018-12-15 00:00:00", "%Y-%m-%d %H:%M:%S");

        model.Run(begin, end);

        const Output& output = model.Get_output();

        double x = output.Get_psi_leaf()[48*20];
        double y = output.Get_psi_leaf()[48*100];

        Analysis_Swiss analysis(&model, swiss_drought_tress, params);
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
}

