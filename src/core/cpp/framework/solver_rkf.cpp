//
// Created by Phillip on 26.04.24.
//

#include "solver_rkf.h"
#include <iostream>

Solver_RKF::Solver_RKF(const Parameters &params) : Water_Potential_Solver(params) {

}

void Solver_RKF::Init_solver() {
    init_base();

    tolerance = 1E-7;

    dts = params.dts;
}


double Solver_RKF::d_psi_leaf(double psi_leaf, double psi_stem) {
    // Update beta parameter that rescales stomatal conductance
    // beta = 0 -> stomata closed; beta = 1 -> stomata fully open
    // The intial function was a simple logic model
    //beta_stom_cond = 1.0 / (1.0 + std::exp(-params.d_50_close * (psi_leaf - params.psi_leaf_50_close)));
    // Gompertz function
    beta_stom_cond = std::exp(-1.0 * std::exp(-1.0 *params.d_50_close*(psi_leaf - psi_gomp_50)));

    // Medlyn phoytosynthesis model
    // mol CO2 s-1 m-2
    gs = params.g0 + beta_stom_cond * (1.0 + params.g1 / std::sqrt(vpd / pressure)) * anet / ca;

    // Convert from Mol CO2 to Mol H2O
    gs *= 1.6;

    // Calculate transpiration
    T = gs * params.leaf_area_index * vpd / pressure;

    // Update stem water flow
    //J = stem_flow_module->Get_Stem_flow(psi_stem, psi_leaf);

    // Return the derivative of the leaf water potential
    return ((J - T)/ params.leaf_hydraulic_capacitance);
}
double Solver_RKF::d_psi_stem_ground(double psi_leaf, double psi_stem) {
    // The water stem flow could be also updated right here, but this might lead to inconsistent water uptakes/
    // in case it is updated two times.


    G = 0.0;
    for (int s = 0; s < params.soil_layers.size(); ++s) {

        const Soil_layer& sl = params.soil_layers[s];

        // Convert k soil from volume to mass flow:
        double k_soil = k_soil_sl[s] * params.constants.RHO_WATER;

        // Convert from kg to mol H2O
        k_soil *= params.constants.KG_H2O_To_Mol;

        // Convert from MPA to hydraulic head
        double psi_stem_hh = psi_stem * params.constants.MPaToHydraulicHeadM;

        Gi[s] = sl.root_fraction * k_soil * std::sqrt(params.root_area_index) /
                params.constants.PI / sl.depth * (psi_soil_sl[s] - psi_stem_hh);

        // Avoid water from flowing down from the stem via roots to the soil
        if (Gi[s] < 0.0)
            Gi[s] = 0.0;

        G += Gi[s];
    }

    // Return the derivative of the stem water potential
    return ((G - J) / (params.stem_hydraulic_capacitance_max * params.canopy_height * params.huber_value));
}

void Solver_RKF::Update_water_potentials() {

    dts_remain = dts;
    h = (int)dts/10.0;

    // Ensure that h is at least one second
    if (h < 1){
        h = 1;
    }

    while(dts_remain > 0){

        double new_h_psi_leaf = update_psi_leaf();
        double new_h_psi_stem_ground = update_psi_stem_ground();

        double h_new = std::min(new_h_psi_leaf, new_h_psi_stem_ground);

        h = h_new;

//        if (h < 1){
//            h = 1;
//        }

        if(h > dts_remain){
            h = dts_remain;
        }

        update_psi_leaf();
        if (psi_leaf_4 > psi_stem_ground){

//            psi_leaf = psi_stem_ground;
//            update_psi_leaf();

            psi_leaf = psi_leaf_4;
        }

//        else if(std::abs(psi_leaf_4 - psi_leaf) > h*0.5/1800.0){
//            psi_leaf = psi_stem_ground;
//            update_psi_leaf();
//
//            psi_leaf = psi_leaf_4;
//        }

        else{
            psi_leaf = psi_leaf_4;
        }

        update_psi_stem_ground();

        if (psi_stem_4 > 0.0){
            psi_stem_ground = psi_leaf_4;
            update_psi_stem_ground();

            if (psi_stem_4 > 0.0){
                // Do not update last vlaue
                psi_stem_ground = -1E-10;
            }
            else{
                psi_stem_ground = psi_stem_4;
            }
        }
        else{
            psi_stem_ground = psi_stem_4;
        }

        dts_remain-= h;
    }

    if (params.verbose) {
        if (psi_leaf < -18){
            double x = 3;
        }
        std::cout << "Psi leaf  " << psi_leaf;
        std::cout << " Psi stem ground  " << psi_stem_ground << std::endl;
    }

}



double Solver_RKF::update_psi_leaf() {

    double dpsi = psi_leaf;
    const double k1 = h * d_psi_leaf(dpsi, psi_stem_ground);
    dpsi = psi_leaf + a21 * k1;
    const double k2 = h * d_psi_leaf(dpsi, psi_stem_ground);
    dpsi = psi_leaf + a31 * k1 + a32*k2;
    const double k3 = h * d_psi_leaf(dpsi, psi_stem_ground);
    dpsi = psi_leaf + a41 * k1 + a42*k2 + a43* k3;
    const double k4 = h * d_psi_leaf(dpsi, psi_stem_ground);
    dpsi = psi_leaf + a51* k1 + a52*k2 +a53 * k3 + a54 * k4;
    const double k5 = h * d_psi_leaf(dpsi, psi_stem_ground);
    dpsi = psi_leaf + a61 * k1 + a62* k2 + a63 * k3 + a64 * k4 + a65 * k5;
    const double k6 = h * d_psi_leaf(dpsi, psi_stem_ground);

    psi_leaf_4 = psi_leaf
                              + d1 * k1
                              + d3 * k3
                              + d4 * k4
                              + d5 * k5;

    const double psi_leaf_5 = psi_leaf
                              + e1 * k1
                              + e3 * k3
                              + e4 * k4
                              + e5 * k5
                              + e6 * k6;

    double frac_step_psi_leaf = std::pow((tolerance * h) / (2.0 * std::abs(psi_leaf_5 - psi_leaf_4)), 1.0 / 4.0);

    return h*frac_step_psi_leaf;
}

double Solver_RKF::update_psi_stem_ground() {
    double dpsi = psi_stem_ground;
    const double k1 = h * d_psi_stem_ground(psi_leaf, dpsi);
    dpsi = psi_stem_ground + a21 * k1;
    const double k2 = h * d_psi_stem_ground(psi_leaf, dpsi);
    dpsi = psi_stem_ground + a31 * k1 + a32*k2;
    const double k3 = h * d_psi_stem_ground(psi_leaf, dpsi);
    dpsi = psi_stem_ground + a41 * k1 + a42*k2 + a43* k3;
    const double k4 = h * d_psi_stem_ground(psi_leaf, dpsi);
    dpsi = psi_stem_ground + a51* k1 + a52*k2 +a53 * k3 + a54 * k4;
    const double k5 = h * d_psi_stem_ground(psi_leaf, dpsi);
    dpsi = psi_stem_ground + a61 * k1 + a62* k2 + a63 * k3 + a64 * k4 + a65 * k5;
    const double k6 = h * d_psi_stem_ground(psi_leaf, dpsi);

    psi_stem_4 = psi_stem_ground
                              + d1 * k1
                              + d3 * k3
                              + d4 * k4
                              + d5 * k5;

    const double psi_stem_5 = psi_stem_ground
                              + e1 * k1
                              + e3 * k3
                              + e4 * k4
                              + e5 * k5
                              + e6 * k6;

    double frac_step_psi_leaf = std::pow((tolerance * h) / (2.0 * std::abs(psi_stem_5 - psi_stem_4)), 1.0 / 4.0);

    return h*frac_step_psi_leaf;
}

Solver_RKF::~Solver_RKF() {

}

