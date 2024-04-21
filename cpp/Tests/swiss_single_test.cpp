//
// Created by Phillip on 27.07.23.
//

#include "swiss_single_test.h"
#include <iostream>
#include <string>


#include "../src/framework/parameters.h"
#include "../src/io/input_swiss.h"
#include "../src/io/input_swiss_mult_soils.h"
#include "../src/modules/model.h"
#include <chrono>
#include "../src/framework/parameter_csv_reader.h"
#include "../src/io/analysis_swiss.h"
#include "../src/io/swiss_drought_trees.h"

using std::cout;
using std::endl;
using std::string;

Swiss_Single_Test::Swiss_Single_Test() {


//    string theta_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Water_content_MultiSoils.csv";
//    string forcing_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Forcing_Inter.csv";
//    string parameters_list = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/py/appl/LHS/SwissParameterList100.csv";
//    string tree_folder_path = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Trees";


    string theta_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Water_content_MultiSoils.csv";
    string forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Forcing_Inter.csv";
    string parameters_list = "/Users/pp/data/Simulations/A08_Hydraulics_standalone/2024/swiss/LHS/less_ks_more_constrained/Best_Alive_avg.csv";
    string tree_folder_path = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Trees";

    //string forcing_file = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Forcing_Inter.csv";

    Parameter_CSV_Reader reader(parameters_list);
    Swiss_Drought_Trees swiss_drought_tress(tree_folder_path);

    auto start0 = std::chrono::high_resolution_clock::now();

    reader.Parse_Full_Files();

    auto end0 = std::chrono::high_resolution_clock::now();
    auto ms0 = std::chrono::duration_cast<std::chrono::milliseconds>( end0 - start0);
    std::cout << "Elapsed time: " << ms0.count() << " ms\n";


    // Default parameters
    Parameters params = reader.Get_parameter_list()[460];

    //params.k_xylem_sat = 600;

    //params.stem_hydraulic_capacitance_max = 200;

//    params.huber_value  = 1.0/3600.0;
//    params.canopy_height  = 20.0;
//    params.g0 = 0.005;
//    params.g1 = 1.5;
//    params.psi_leaf_50_close = -2.1;
//    params.d_50_close = 10.0;
//    params.leaf_area_index = 4.8;
//    params.leaf_hydraulic_capacitance = 1.0;
//    params.stem_hydraulic_capacitance_max =1 * 1000 / 18.0;
//    params.k_xylem_sat = 300;
//
//    params.root_area_index = 10;
//
//    params.jackson_root_beta = 0.8;
//    params.theta_r = 0.0972;
//    params.alpha_genucht = 1.0;
//    params.n_genucht = 5.0;
//    params.neta_genucht = 0.5;
//
//    params.theta_s = 0.43;
//    params.camp_b = 4.5;
//    params.camp_psi_soil_ref = -4.5E-3;
//
//    params.psi50_xylem = -3.5;
//    params.psi88_xylem = -5.0;
//
//    params.soil_depths[0] = 0.1;
//    params.soil_depths[1] = 0.3;
//    params.soil_depths[2] = 0.4;
//
//    params.k_soil_sats[0] = 1.0/100.0/86400.0 * 0.000001;
//    params.k_soil_sats[1] = 1.0/100.0/86400.0 * 0.00001;
//    params.k_soil_sats[2] = 1.0/100.0/86400.0 * 0.00001;
//
//    params.clay_fracs[0] = 0.619;
//    params.clay_fracs[1] = 0.329;
//    params.clay_fracs[2] = 0.286;
//
//    params.sand_fracs[0] = 0.12;
//    params.sand_fracs[1] = 0.254;
//    params.sand_fracs[2] = 0.251;
//
//    params.organic_matter_fracs[0] = 0.06;
//    params.organic_matter_fracs[1] = 0.06;
//    params.organic_matter_fracs[2] = 0.06;
//
//    params.soil_profile_index = 4;
//
//    params.stem_flow_type = Stem_flow_module_type::Linear;

    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.3;

    Input_Swiss_Multi_Soils input(    params);
    input.Add_Forcing_File(forcing_file);
    input.Add_Soilwater_File(theta_file);
    input.Read_N_Parse();

    auto start_clock = std::chrono::high_resolution_clock::now();

    Leaf_Stem_Implicit_Model model(params, input);

    model.Set_derived_parameters();

    model.Set_initial_conditions(psi_leaf_init, psi_stem_init);

    // Length model in seconds
    long steplen      = 1800;

//    double timestart    = 30*2*24 * 0.0;
//    double timeend      = 30*2*24 * 213;

    DateTime begin = input.dates.front();
    DateTime end = input.dates.back();

    model.Run(steplen, begin, end);

    Analysis_Swiss analysis(&model, swiss_drought_tress);
    analysis.Run();


    auto end_clock = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_clock - start_clock);
    std::cout << "Elapsed time: " << ms.count() << " ms\n";

}
