//
// Created by Phillip on 25.04.24.
//

#include "solver_indiv_eulerimp.h"
#include <iostream>

Solver_Indiv_Euler_Imp::Solver_Indiv_Euler_Imp(const Parameters &params) : Water_Potential_Solver(params) {
    // Initialise with no stress on stomatal conductance
    beta_stom_cond = 1.0;
}

void Solver_Indiv_Euler_Imp::Update_water_potentials() {

    // Within the standard routine, we calculate J based on leaf water potential and then
    // use the fixed J for the stem water potential routine
    calc_J_stem = false;
    calc_J_leaf = true;

    // Leaf water solving routine
    // If we have an  unrealistic leaf water potential, stop the solver
    // and assume the plant is dead. Set all fluxes to zero
    if (psi_leaf < min_leaf_water_potential  + 0.5){
        psi_leaf = min_leaf_water_potential;

        T = 0.0;
        J = 0.0;
        G = 0.0;
    }

    // Regular solver solver routine
    else {
        update_psi_leaf();

        if (psi_leaf > min_leaf_water_potential + 0.5){
            update_psi_stem_ground();
        }

    }
    calc_psi_stems();
    stem_flow_module->Update_min_conductivity_fractions();

    psi_leaf_prev_ts = psi_leaf;
    psi_stem_ground_prev_ts = psi_stem_ground;

    // Update_photosythesis beta parameter that rescales stomatal conductance
    // beta = 0 -> stomata closed; beta = 1 -> stomata fully open
    beta_stom_cond = std::exp(-1.0 * std::exp(-1.0 *params.d_50_close*(psi_leaf - psi_gomp_50)));

    if (params.verbose){
        std::cout << " Psi leaf  " << psi_leaf;
        std::cout << " Psi stem ground  " << psi_stem_ground << std::endl;
        std::cout <<   "T : " << T;
        std::cout <<   " J : " << J;
        std::cout << " G: " << G << std::endl;
        std::cout << " T_G: " << T_G << std::endl;
        std::cout <<   " VPD : " << vpd ;
        std::cout <<   " gs : " << gs ;
        std::cout << " psi_soil: " << psi_soil_sl[0]*params.constants.HydraulicHeadInMtoMPa;
        std::cout << " K_soil: " << k_soil_sl[0] << std::endl;
        std::cout << ""<< std::endl;
    }

}

void Solver_Indiv_Euler_Imp::Init_solver() {

    init_base();

    // Initialise solvers
    solver_psi_leaf = std::make_unique<Bisection_psi_leaf>(*this,
                                                           1E-10,
                                                           100);

    solver_psi_stem_ground = std::make_unique<Bisection_psi_stem_ground>(*this,
                                                                         1E-10,
                                                                         100);
    calc_J_leaf = true;
    calc_J_stem = false;

}

double Solver_Indiv_Euler_Imp::psi_stem_root(double psi_stem_target) {
    double d_psi_stem_rec = d_psi_stem_ground(psi_leaf, psi_stem_target);
    return psi_stem_ground + d_psi_stem_rec * dts - psi_stem_target;
}

double Solver_Indiv_Euler_Imp::psi_leaf_root(double psi_leaf_target) {
    double d_psi_leaf_rec = d_psi_leaf(psi_leaf_target, psi_stem_ground);
    return psi_leaf + d_psi_leaf_rec * dts - psi_leaf_target;
}


double Solver_Indiv_Euler_Imp::d_psi_leaf(double psi_leaf, double psi_stem) {

    // Update_photosythesis transpiration
    T = update_transpiration();

    // Update_photosythesis stem water flow
    if (calc_J_leaf){
        J = stem_flow_module->Get_Stem_flow(psi_stem, psi_leaf);
    }

    // Return the derivative of the leaf water potential
    return ((J - T)/ params.leaf_hydraulic_capacitance);
}
double Solver_Indiv_Euler_Imp::d_psi_stem_ground(double psi_leaf, double psi_stem) {
    // The water stem flow could be also updated right here, but this might lead to inconsistent water uptakes/
    // in case it is updated two times.

    // Update_photosythesis stem water flow
    if (calc_J_stem){
        J = stem_flow_module->Get_Stem_flow(psi_stem, psi_leaf);
    }

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

    // Bark water loss
    T_G = params.g_bark * vpd / pressure;

    // Return the derivative of the stem water potential
    return ((G - J - T_G) / (params.stem_hydraulic_capacitance_max * params.canopy_height * params.huber_value));
}

double Solver_Indiv_Euler_Imp::update_transpiration() {
    // Calculate transpiration per unit leaf area
    return gs * vpd / pressure * params.constants.H2O_TO_CO2_DIFFUSIVITY;
}


// -----------------------------------------------------------------------------------------------------
//
//  Stem water potential solving routine
//
// -----------------------------------------------------------------------------------------------------
void Solver_Indiv_Euler_Imp::update_psi_stem_ground() {

    // For given leaf water potential AND stem water flow J (esimated by the leaf water potential routen)
    // Solve for stem water potential: Minmum value is the leaf water potential and the maximum value is the
    // maximum stem water potential (close to 0.0)
    bool converged = solver_psi_stem_ground->Solve(psi_leaf, params.constants.MAX_STEM_WATER_POTENTIAL);

    // Normal routine: Stem water potential found in rang
    if (converged){
        psi_stem_ground = solver_psi_stem_ground->Get_solution();
    }



    // Stem water potential cannot fulfill the previously estimated J by leaf water potential. Therefore, the solution
    // would require stem water potential to be more negative than leaf water potential. This is of course
    // impossible, and we need to restimate leaf water potential taking the limitation by the stem into account
    else {
        // Therefore, calculate stem water potential AND the stem water flow
        // assuming constant leaf water potential (from timestep before), but variable stem water flow J
        calc_J_stem = true;
        converged = solver_psi_stem_ground->Solve(psi_leaf_prev_ts, params.constants.MAX_STEM_WATER_POTENTIAL );


        // We found a solution of stem water flow
        if (converged) {
            psi_stem_ground = solver_psi_stem_ground->Get_solution();

            //  Calculate leaf water potential based on J estimated by the stem water potential model
            calc_J_leaf = false;
            converged = solver_psi_leaf->Solve(0.0, psi_leaf_prev_ts - max_psi_leaf_change_per_ts);

            // Found a new solution which prob. more negative than the previous esimated psi_leaf
            if (converged){
                psi_leaf = solver_psi_leaf->Get_solution();
            }

            // Could not determine solution of leaf water potential.
            // Set solution to maximum negative drop
            else{
                psi_leaf = psi_leaf_prev_ts -  max_psi_leaf_change_per_ts;
                T = update_transpiration();
                calc_J_stem = true;

                // Re-estimate psi_stem AND J
                converged = solver_psi_stem_ground->Solve(psi_leaf, params.constants.MAX_STEM_WATER_POTENTIAL);
                psi_stem_ground = solver_psi_stem_ground->Get_solution();

                if (!converged) {
                    std::cout << "Could not determine stem water potential between " << psi_leaf << " and ";
                    std::cout << params.constants.MAX_STEM_WATER_POTENTIAL;
                    std::cout << "with variable J depending on psi_stem." << std::endl;
                    std::cout << "This situation is considered quite unlikely" << std::endl;
                    std::cout << "ID of parameter file: " << params.id << std::endl;

                    if (psi_leaf < params.psi88_xylem){
                        std::cout << "Leaf water potential is already quite negative" << std::endl;
                        std::cout << "Assuming plant is dead" << std::endl;
                        psi_leaf = min_leaf_water_potential;
                    }

                    else if(std::abs(psi_leaf_prev_ts - psi_stem_ground_prev_ts) < 0.1){
                        std::cout << "Leaf water potential and stem water potential are very close" << std::endl;
                        std::cout << "Do not update T and G and leave leaf water potential as is" << std::endl;
                        std::cout << "Set J to zero" << std::endl;
                        psi_stem_ground = psi_stem_ground_prev_ts;
                        J = 0.0;
                    }

                    else{
                        std::cout << "Leaf water potential is still in well functioning range." << std::endl;
                        std::cout << "This problem deserves a deeper investigation..." << std::endl;
                        //std::cout << "Terminate program as we cannot ensure physical reliability of the model." << std::endl;
                        //exit(99);
                        std::cout << "1: Set J to zero" << std::endl;
                        J = 0.0;
                    }
                }
            }

//            // No solution within the full leaf water potential boundaries found
//            // Assume that we have a huge drop in leaf water potential and set to minimum value
//            if (!converged) {
//                std::cout << "Could not determine leaf water potential between " << min_leaf_water_potential << " and ";
//                std::cout << params.max_leaf_water_potential << std::endl;
//                std::cout << "Assuming plant is dead and setting to minimum value: " << min_leaf_water_potential
//                          << std::endl;
//                psi_leaf = min_leaf_water_potential;
//                return;
//            }
//            calc_J_stem = false;
//
//            // 3. Re-Calculate stem water potential based on new leaf water potential and previously esimated J
//            converged = solver_psi_stem_ground->Solve(psi_leaf, params.max_stem_water_potential);
//            if (converged) {
//                psi_stem_ground = solver_psi_stem_ground->Get_solution();
//                calc_J_leaf = true;
//            }

//            else {
//                std::cout << "Could not determine stem water potential between new " << psi_leaf << " and ";
//                std::cout << params.max_leaf_water_potential << std::endl;
//                std::cout << "Stem water potential still to be more negative compared to previous psi_leaf" << std::endl;
//                std::cout << "This problem cannot be solved at the moment" << std::endl;
//                std::cout << "contact papa@bgc-jena.mpg.de" << std::endl;
//                std::cout << "Exiting.."<< std::endl;
//                exit(99);
//            }


        }
        else {
 //          std::cout << "Could not determine stem water potential between " << psi_leaf << " and ";
//            std::cout << params.constants.MAX_STEM_WATER_POTENTIAL;
//            std::cout << "with variable J depending on psi_stem." << std::endl;
//            std::cout << "This situation is considered quite unlikely" << std::endl;
//            std::cout << "ID of parameter file: " << params.id << std::endl;

            if (psi_leaf < params.psi88_xylem){
                std::cout << "Leaf water potential is already quite negative" << std::endl;
                std::cout << "Assuming plant is dead" << std::endl;
                psi_leaf = min_leaf_water_potential;
            }

            else if(std::abs(psi_leaf_prev_ts - psi_stem_ground_prev_ts) < 0.1){
//                std::cout << "Leaf water potential and stem water potential are very close" << std::endl;
//                std::cout << "Do not update T and G and leave leaf water potential as is" << std::endl;
//                std::cout << "Set J to zero" << std::endl;
                J = 0.0;
            }

            else{
                std::cout << "Could not determine stem water potential between " << psi_leaf << " and ";
                std::cout << params.constants.MAX_STEM_WATER_POTENTIAL;
                std::cout << "with variable J depending on psi_stem." << std::endl;
                std::cout << "This situation is considered quite unlikely" << std::endl;
                std::cout << "ID of parameter file: " << params.id << std::endl;
                std::cout << "Leaf water potential is still in well functioning range." << std::endl;
                std::cout << "This problem deserves a deeper investigation..." << std::endl;
                //std::cout << "Terminate program as we cannot ensure physical reliability of the model." << std::endl;
                std::cout << "2: Set J to zero" << std::endl;
                J = 0.0;
                //exit(99);
            }
        }
    }

}

// -----------------------------------------------------------------------------------------------------
//
//  Leaf water potential solving routine
//
// -----------------------------------------------------------------------------------------------------


void Solver_Indiv_Euler_Imp::update_psi_leaf() {

    // Calculate leaf water potential boundaries for bisection solver
    // This step should enable quicker convergence
    double psi_leaf_lower = psi_leaf - max_psi_leaf_change_per_ts;
    double psi_leaf_upper = psi_leaf + max_psi_leaf_change_per_ts;

    // Constrain leaf water potential to a maximum value close to 0.0;
    // Avoiding positive values of psi_leaf
    psi_leaf_upper = std::min(psi_leaf_upper, params.constants.MAX_LEAF_WATER_POTENTIAL);


    // Solve for leaf water potential...
    bool converged = solver_psi_leaf->Solve(psi_leaf_lower, psi_leaf_upper);


    //std::cout <<  "conv  ";

    // We have a solution within the standard boundaries...
    if (converged) {
        psi_leaf = solver_psi_leaf->Get_solution();
    }



    // ... we have NO solution within the standard boundaries...
    // The solution (if there is one) shows a bigger drop then the maximum anticipated by the standard routine
    else {

        // Check again if leaf water potential is recovering quicker than the standard boundaries allow
        // We allow this to happen, as it much less problematic thant a more negative drop in leaf water potential
        bool converged = solver_psi_leaf->Solve(psi_leaf, params.constants.MAX_LEAF_WATER_POTENTIAL);
        if (converged) {
            psi_leaf = solver_psi_leaf->Get_solution();
        }

        // Leaf water potential drops more rapidly (to a more negative value) then anticipated by the standard library
        // Set to maximum drop and calculate transpiration with fixed psi leaf.
        // Also do not calculate the stem water flow but rather let the stem routine esimate the water flow directly
        else{
            psi_leaf -= max_psi_leaf_change_per_ts;
            T = update_transpiration();
            calc_J_stem = true;
        }

//        double x_max = params.psi50_xylem;
//        converged = solver_psi_leaf->Solve(0.0, x_max);
//
//        if (converged)
//            psi_leaf = solver_psi_leaf->Get_solution();
//
//        else{
//            converged = solver_psi_leaf->Solve(0.0, min_leaf_water_potential);
//            if (converged)
//                psi_leaf = solver_psi_leaf->Get_solution();
//        }
//
//        // No solution within the full leaf water potential boundaries found
//        // Assume that we have a huge drop in leaf water potential and set to minimum value
//        if (!converged) {
//            std::cout << "Could not determine leaf water potential between " << min_leaf_water_potential << " and ";
//            std::cout << params.max_leaf_water_potential << std::endl;
//            std::cout << "Assuming plant is dead and setting to minimum value: " << min_leaf_water_potential
//                      << std::endl;
//            psi_leaf = min_leaf_water_potential;
//        }


    }

}


double Solver_Indiv_Euler_Imp::Get_beta() {
    return beta_stom_cond;
}


double Bisection_psi_leaf::f(double x) {
    return model.psi_leaf_root(x);
}

double Bisection_psi_stem_ground::f(double x) {
    return model.psi_stem_root(x);
}

