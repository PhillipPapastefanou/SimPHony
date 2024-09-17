//
// Created by Phillip on 25.04.24.
//

#include <iostream>
#include "water_potential_solver.h"
#include "../modules/root_distribution_model.h"


//Water_Potential_Solver::Water_Potential_Solver(const Parameters &params): params(params) {
//
//}

Water_Potential_Solver::Water_Potential_Solver(const Parameters& params): params(params){

}

void Water_Potential_Solver::Init_water_potentials(double psi_leaf_init, double psi_stem_base_init) {

    psi_leaf = psi_leaf_init;
    psi_stem_ground = psi_stem_base_init;

    psi_leaf_prev_ts = psi_leaf_init;
    psi_stem_ground_prev_ts = psi_stem_base_init;
}



void Water_Potential_Solver::init_base() {

    dts = params.dts;

    // Calculate minimum leaf water potential
    min_leaf_water_potential = params.psi88_xylem * params.minimum_psi_leaf_multiplier;
    max_psi_leaf_change_per_ts = params.max_psi_leaf_change_per_hour * dts / params.constants.SEC_IN_HOUR;

    Gi.resize(params.soil_layers.size());

    switch (params.stem_flow_type) {

        case Stem_flow_module_type::Linear:{
            stem_flow_module = std::make_unique<Linear_Segmented_flow>(params);
            break;
        }
        case Stem_flow_module_type::KirchhoffWeibull:{
            stem_flow_module = std::make_unique<Kirchhoff_Weibull_stem_flow>(params);
            break;
        }
        case Stem_flow_module_type::KirchhoffPiecewiseErf:{
            stem_flow_module = std::make_unique<Kirchhoff_Piecewise_Erf>(params);
            break;
        }
        default:{
            std::cout << "Invalid soil water uptake" << std::endl;
            exit(99);
        }
    }
    stem_flow_module->Init();
    psi_stem_segments.resize(params.n_stem_segments);

    // Gompertz function parameter estimates
    psi_gomp_50 = params.psi_leaf_50_close;
    psi_gomp_50 += std::log(std::log(2.0)) / params.d_50_close;


}


void Water_Potential_Solver::Update_forcing(std::vector<double> psi_soil_sl, std::vector<double> k_soil_sl, double anet,
                                            double vpd, double ca, double pressure) {

    this->psi_soil_sl = psi_soil_sl;
    this->k_soil_sl = k_soil_sl;

    this->anet = anet;
    this->vpd = vpd;
    this->ca = ca;
    this->pressure = pressure;
}

void Water_Potential_Solver::calc_psi_stems() {

    // Water potential drop per stem segment
    double delta_psi_per_segment = (psi_stem_ground - psi_leaf) / params.n_stem_segments;

    for (int n = 0; n < params.n_stem_segments; ++n) {
        // Calculate lower and upper water potential of each segment
        const double psi_lower_seg = psi_stem_ground - n * delta_psi_per_segment;
        const double psi_upper_seg = psi_stem_ground - (n + 1) * delta_psi_per_segment;

        // Calculate the average water potential between segments [MPa]
        double psi_avg_seg = (psi_lower_seg + psi_upper_seg) / 2.0;
        psi_stem_segments[n] = psi_avg_seg;
    }

}

void Water_Potential_Solver::Update_output(Output &output) {

    output.Add_T(T);
    output.Add_J(J);
    output.Add_G(G);

    vector<float> Gi_f(Gi.begin(), Gi.end());
    for (auto& e: Gi_f)
        e *= 1.0;

    output.Add_G_indiv(Gi_f);
    output.Add_psi_leaf(psi_leaf);
    output.Add_psi_stem_ground(psi_stem_ground);

    vector<float> psi_stems_f(psi_stem_segments.begin(), psi_stem_segments.end());
    output.Add_psi_stems_seg(psi_stems_f);

    output.Add_gs(gs);
    output.Add_beta(beta_stom_cond);

}

Water_Potential_Solver::~Water_Potential_Solver() {

}


