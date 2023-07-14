#include <iostream>
#include <string>


#include "parameters.h"
#include "input.h"
#include "model.h"
#include <chrono>


using std::cout;
using std::endl;

using std::string;

int main(int argc, char* argv[]) {

    string theta_file = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Water_Input_type2.csv";
    string forcing_file = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Forcing_Inter.csv";


    // Default parameters
    Parameters params;

    params.huber_value  = 1.0/3600.0;
    params.canopy_height  = 20.0;
    params.g0 = 0.005;
    params.g1 = 1.5;
    params.psi_leaf_50_close = -2.1;
    params.d_50_close = 10.0;
    params.leaf_area_index = 4.8;
    params.leaf_hytdraulic_capacitance = 1.0;
    params.stem_hydraulic_capacitance = 20.0*1000/18.0;
    params.k_xylem_sat = 300;

    params.root_area_index = 24;
    params.root_zone_depth = 0.3;

    params.nsoil = 11;
    params.layer_depth = 0.1;
    params.min_soil_layer_depth = 0.0;
    params.max_soil_layer_depth = 1.0;
    params.jackson_root_beta = 0.96;
    params.theta_R = 0.0972;
    params.alpha_genucht = 1.0;
    params.n_genucht = 5.0;
    params.neta_genucht = 0.5;


    params.theta_S = 0.8;
    params.camp_b = 8.4;
    params.camp_psi_soil_ref = -1.5E-6;
    params.k_soil_sat = 100.0 * 15.0/86400.0;


    double psi_leaf_init = -1.0;
    double psi_stem_init = -0.3;


    Input input(theta_file,forcing_file);


    input.Read_N_Parse();


    auto start = std::chrono::high_resolution_clock::now();

    Leaf_Stem_Implicit_Model model(params, input);

    model.Set_derived_parameters();

    model.Set_initial_conditions(psi_leaf_init, psi_stem_init);


    double steplen      = 30;
    double timestart    = 30*2*24 * 0.0;
    double timeend      = 30*2*24 * 200;

    model.Run(steplen,timestart,timeend);


    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>( end - start);
    std::cout << "Elapsed time: " << ms.count() << " ms\n";





}
