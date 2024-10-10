//
// Created by Phillip on 13.07.23.
//

#include "param_examples.h"

void Parameter_Examples::Define() {

    Baseline_one.huber_value  = 1.0/3600.0;
    Baseline_one.canopy_height  = 20.0;
    Baseline_one.g0 = 0.005;
    Baseline_one.g1 = 1.5;
    Baseline_one.psi_leaf_50_close = -2.1;
    Baseline_one.d_50_close = 10.0;
    Baseline_one.leaf_area_index = 4.8;
    Baseline_one.leaf_hydraulic_capacitance = 1.0;
    Baseline_one.stem_hydraulic_capacitance_max = 30 * 1000 / 18.0;
    Baseline_one.k_xylem_sat = 300;

    Baseline_one.root_area_index = 24;

    Baseline_one.soil_layers.resize(3);

    Baseline_one.jackson_root_beta = 0.96;

//    Baseline_one.theta_r = 0.033;
//    Baseline_one.pore_size_ind = 0.24985;
//    Baseline_one.theta_s = 0.42685;
//    Baseline_one.camp_b = 10.4;
//    Baseline_one.psi_soil_sat = -1.5E-6;
//
    Baseline_one.wcont_sigma_deviation = 1.0;



    for (int i = 0; i < 3; ++i) {
        Baseline_one.soil_layers[i].k_soil_sat = 9.8084e-06;
    }
}
