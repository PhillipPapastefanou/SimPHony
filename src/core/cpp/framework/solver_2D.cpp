//
// Created by Phillip on 25.04.24.
//

#include "solver_2D.h"
#include <iostream>


void Solver_2D::Update_water_potentials() {

    // If we have an extremely unrealistic leaf water potential, stop the solver
    // Plant is assumed to be dead
    if (psi_leaf < min_leaf_water_potential  + 0.5){
        psi_leaf = min_leaf_water_potential;

        T = 0;
        J = 0;

        if (params.verbose){
            std::cout << "Psi leaf  " << psi_leaf;
            std::cout << " Psi stem ground  " << psi_stem_ground  << std::endl;

            std::cout <<   "T: " << T;
            std::cout << "  J: " << J;
            std::cout << "  G: " << G << std::endl;
        }

        // Adding variables to up output files
        //add_output();
    }

        // Normal solver routine
    else{
        // Convert to maximum change per timestep [MPa]
        const double max_psi_leaf_change_per_ts =  params.max_psi_leaf_change_per_hour * dts / params.constants.SEC_IN_HOUR;

        // Calculate leaf water potential boundaries for bisection solver
        double psi_leaf_lower = psi_leaf - max_psi_leaf_change_per_ts;
        double psi_leaf_upper = psi_leaf + max_psi_leaf_change_per_ts;

        psi_leaf_upper = std::max(psi_leaf_upper, params.constants.MAX_LEAF_WATER_POTENTIAL);
        //psi_leaf = solver_psi_leaf->Solve(psi_leaf_lower, psi_leaf_upper);
    }

    if (params.verbose) {
        std::cout << "Psi leaf  " << psi_leaf;
        std::cout << " Psi stem ground  " << psi_stem_ground << std::endl;

        std::cout <<   "T: " << T;
        std::cout << "  J: " << J;
        std::cout << "  G: " << G << std::endl;
    }

}

double Solver_2D::d_psi_leaf(double psi_leaf, double psi_stem) {

    this->psi_leaf = psi_leaf;

    // Update beta parameter that rescales stomatal conductance
    // beta = 0 -> stomata closed; beta = 1 -> stomata fully open
    // The intial function was a simple logic model
    //beta_stom_cond = 1.0 / (1.0 + std::exp(-params.d_50_close * (psi_leaf - params.psi_leaf_50_close)));
    // Gompertz function
    beta_stom_cond = std::exp(-1.0 * std::exp(-1.0 *params.d_50_close*(this->psi_leaf - psi_gomp_50)));

    // Medlyn phoytosynthesis model
    // mol CO2 s-1 m-2
    gs = params.g0 + beta_stom_cond * (1.0 + params.g1 / std::sqrt(vpd / pressure)) * anet / ca;

    // Convert from Mol CO2 to Mol H2O
    gs *= 44.0/18.0;

    //Bisection_psi_stem_ground bisectionPsiStemGround(*this,1E-10, 100, this->psi_leaf, params.max_leaf_water_potential);
    //psi_stem_ground = bisectionPsiStemGround.Solve(this->psi_leaf, params.max_leaf_water_potential);

    // Calculate transpiration
    T = gs * params.leaf_area_index * vpd / pressure;

    std::cout << this->psi_leaf << " " << psi_stem_ground << std::endl;
    std::cout << T << " " << J << " " << G << std::endl;

    // Return the derivative of the leaf water potential
    return ((J - T)/ params.leaf_hydraulic_capacitance);
}

double Solver_2D::d_psi_stem_ground(double psi_leaf, double psi_stem) {

    // The water stem flow could be also updated right here, but this might lead to inconsistent water uptakes/
    // in case it is updated two times.

    // Update stem water flow
    // J = stem_flow_module->Get_Stem_flow(psi_stem, this->psi_leaf);

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

//double Solver_2D::psi_stem_root(double psi_stem_target) {
//    double d_psi_stem_rec = d_psi_stem_ground(psi_leaf, psi_stem_target);
//    return psi_stem_ground + d_psi_stem_rec * dts - psi_stem_target;
//}
//
//double Solver_2D::psi_leaf_root(double psi_leaf_target) {
//    double d_psi_leaf_rec = d_psi_leaf(psi_leaf_target, psi_stem_ground);
//    return psi_leaf + d_psi_leaf_rec * dts - psi_leaf_target;
//}
//


Solver_2D::Solver_2D(const Parameters &params) : Water_Potential_Solver(params
        ) {

}

void Solver_2D::Init_solver() {

    init_base();
}

//    // Initialise solvers
//    solver_psi_leaf = std::make_unique<Bisection_psi_leaf>(*this,
//                                                           1E-10,
//                                                           100,
//                                                           min_leaf_water_potential,
//                                                           params.max_leaf_water_potential);
//
//    solver_psi_stem_ground = std::make_unique<Bisection_psi_stem_ground>(*this,
//                                                                         1E-10,
//                                                                         100,
//                                                                         min_leaf_water_potential,
//                                                                         params.max_leaf_water_potential);
//}

void Solver_2D::balance_ponts(double x_min, double x_max, double y_min, double y_max, double delta) {

    double width = x_max - x_min;
    double height = y_max - y_min;
    double x_middle = (x_min + x_max)/2;
    double y_middle = (y_min + y_max)/2;


//    double fA = psi_leaf_root_2D(x_min, y_min);
//    double fB = psi_leaf_root_2D(x_max, y_min);
//    double fC = psi_leaf_root_2D(x_min, y_max);
//    double fD = psi_leaf_root_2D(x_max, y_max);
//
//    double gA = psi_stem_root_2D(x_min, y_min);
//    double gB = psi_stem_root_2D(x_max, y_min);
//    double gC = psi_stem_root_2D(x_min, y_max);
//    double gD = psi_stem_root_2D(x_max, y_max);
//
//    double Fx = std::min(fC*fB,fD*fA);
//    double Fy = std::min(fA*fB,fD*fC);
//
//    double Gx = std::min(gC*gB,fD*fA);
//    double Gy = std::min(gA*gB,gD*gC);
//
//    double F = std::min(Fx, Fy);
//    double G = std::min(Gx, Gy);
//
//    double max = std::max(F, G);
//
//    double largest_dim = std::max(width, height);
//
//    bool guaranteed_contain_zeros = std::max(F, G) < 0;

//    if (guaranteed_contain_zeros && (largest_dim <= 1E-6)) {
////        x_middle_z = x_middle;
////        y_middle_z = y_middle;
////    }
//    else if(guaranteed_contain_zeros || (largest_dim > delta)){
//        balance_ponts(x_min, x_middle, y_min, y_max, delta);
//    }
//    else{
//        balance_ponts(x_min, x_max, y_min, y_middle, delta);
//        balance_ponts(x_min, x_max, y_middle, y_max, delta);
//    }
}

