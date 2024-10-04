//
// Created by ppapastefanou on 4/8/24.
//
#include "stem_flow_model.h"
#include <iostream>
#include "../auxil/amath.h"

Stem_flow_module::Stem_flow_module(const Parameters &params) :
        params(params),
        gravity(params.constants.GRAVITY),
        rho_water(params.constants.RHO_WATER),
        MPaToPa(params.constants.MPaToPa),
        PA_TO_MPA(params.constants.PaToMPa){

    min_frac_con_per_segment.resize(params.n_stem_segments);
    // Setting the fraction of healthy xylem to 100% at the beginning
    for (auto& value:min_frac_con_per_segment ) {
        value = 1.0;
    }
    actual_frac_con_per_segment.resize(params.n_stem_segments);
}

Stem_flow_module::~Stem_flow_module() {
}

void Stem_flow_module::Update_min_conductivity_fractions() {

    for (int n = 0; n < params.n_stem_segments; ++n) {
        min_frac_con_per_segment[n] = std::min(min_frac_con_per_segment[n], actual_frac_con_per_segment[n]);
    }
}


Linear_Segmented_flow::Linear_Segmented_flow(const Parameters &params) : Stem_flow_module(params), k_xylem_loss_table() {
}

void Linear_Segmented_flow::Init(){

    double psi_min = params.psi88_xylem * 4.0;
    double psi_max = 0.0;
    double delta_psi_step = 0.01;


    switch (params.conductivity_fraction_type) {
        case Conductivity_fraction_module_type::Weibull: {
            conductivity_module = std::make_unique<WeibullCDF>(params);
            break;
        }

        case Conductivity_fraction_module_type::Logit: {
            conductivity_module = std::make_unique<Logit>(params);
            break;
        }
        default:{
            std::cout << "Invalid conductivity module specified" << std::endl;
            exit(99);
        }
    }
    conductivity_module->Init();

    std::vector<double> psi_values;
    std::vector<double> k_loss_values;

    for (double psi = psi_min; psi < psi_max; psi += delta_psi_step) {
        psi_values.push_back(psi);
        k_loss_values.push_back(conductivity_module->Get_fraction(psi));
    }
    k_xylem_loss_table.Init(psi_values, k_loss_values);
}


double Linear_Segmented_flow::Get_Stem_flow(double psi_stem_ground, double psi_leaf) {

    // Water potential drop per stem segment
    double delta_psi_per_segment = (psi_stem_ground - psi_leaf) / params.n_stem_segments;

    // If the differences of the leaf water potential and the psi bottom layer are zero
    // assume no water flow
    if (std::abs(delta_psi_per_segment) < 1E-12){
        return 0.0;
    }

    // Height of each segment [m]
    double segment_height =  params.canopy_height / params.n_stem_segments;

    // Hydrostatic perssure per segment [MPa]
    double psi_hydrostatic_per_segment = (rho_water * gravity *segment_height) * PA_TO_MPA;

    // Total water flow through the stem [mol m-2 s-1]
    double stem_water_flow = 0.0;

    // Loop through segments
    // Starting from the lowest segment and iterate until we reach the top
    for (int n = 0; n < params.n_stem_segments; ++n) {

        // Calculate lower and upper water potential of each segment
        const double psi_lower_seg = psi_stem_ground - n * delta_psi_per_segment;
        const double psi_upper_seg = psi_stem_ground - (n + 1) * delta_psi_per_segment;

        // Calculate the average water potential between segments [MPa]
        double psi_avg_seg = (psi_lower_seg + psi_upper_seg) / 2.0;

        // Loss of hydraulic conductivity per segment [0-1]
        actual_frac_con_per_segment[n] = k_xylem_loss_table.Get(psi_avg_seg);

        if (params.sustain_xylem_damage){
            double min_fraction = std::min(min_frac_con_per_segment[n], actual_frac_con_per_segment[n]);
            actual_frac_con_per_segment[n] = min_fraction;
        }

        // Water uptake is the difference between the segments minus the hydrostatic pressure [MPA]
        double delta_psi_uptake = delta_psi_per_segment - psi_hydrostatic_per_segment;
        // Prevent negative pressure differences to avoid letting the water flow down the tree
        if(delta_psi_uptake < 0.0)
            delta_psi_uptake = 0.0;

        // Accumulate stem water flow per segment
        stem_water_flow += delta_psi_uptake * actual_frac_con_per_segment[n] * params.k_xylem_sat;
    }

    // Multiply with constants
    stem_water_flow *= params.huber_value / (params.eta_LS * params.canopy_height);

    return stem_water_flow;
}

Kirchhoff_Weibull_stem_flow::Kirchhoff_Weibull_stem_flow(const Parameters &params) : Stem_flow_module(params) {


}

void Kirchhoff_Weibull_stem_flow::Init() {

    // Obtain Weibull parameters from psi50 and psi88
    double x1 = 0.5;
    double x2 = 0.88;

    c = log(log(1.0 - x1)/log(1.0 - x2))/(log(-params.psi50_xylem) - log(-params.psi88_xylem));
    b = -params.psi50_xylem/std::pow(-log(1.0 - x1), (1.0/c));

}

double Kirchhoff_Weibull_stem_flow::KirchhoffIntegral(double psi) {
    double tau = psi * std::pow(std::pow(psi/b, c), -1.0/c);
    double ulam = (1.0 - gamma_inc(1.0/ c, std::pow(psi/b, c))) * std::tgamma(1.0/c);
    return -1.0 *  tau * ulam / c;
}

double Kirchhoff_Weibull_stem_flow::Get_Stem_flow(double psi_stem, double psi_leaf) {

    // Calculate hydrostatic pressure
    double psi_hydro = (rho_water * gravity * params.canopy_height) * PA_TO_MPA;

    // If leaf wand soil water potential are (almost) identical we avoid the divide by zero calcuation and
    // return zero water flow
    if (std::abs(psi_stem - psi_leaf) < 1E-12){
        return 0.0;
    }

    // Account for the effect of the hydrostatic pressure according to equation xxx
    double flow_psi_coeff = (1.0 - psi_hydro/(psi_stem - psi_leaf));

    // We avoid water flowing down the tree for now
    if (flow_psi_coeff < 0.0){
        return 0.0;
    }

    // Solving the conductivity integral
    double J_unit = KirchhoffIntegral(- psi_leaf) - KirchhoffIntegral(- psi_stem);

    return J_unit * params.k_xylem_sat * flow_psi_coeff * params.huber_value / (params.eta_LS * params.canopy_height);
}

double Kirchhoff_Piecewise_Erf::g_erf(double psi50, double slope, double psi_q, double q) {
    return 0.5* std::erf(slope * (psi_q - psi50)) + 0.5 - q;
}

double Kirchhoff_Piecewise_Erf::FindKirchhoffSlope(double psi50, double psi_q, double q) {
    double s0 = 0;
    double s1 = 1000;

    // Actucally not neccessary to initialise
    double s2 = (s0 + s1) / 2.0;

    bool not_converged = true;
    int step = 0;

    while(not_converged){

        s2 = (s0 + s1) / 2.0;

        double y0 = g_erf(psi50, s0, psi_q, q );
        double y2 = g_erf(psi50, s2, psi_q, q );

        if(y0 * y2 < 0.0){
            s1 = s2;
        }
        else{
            s0 = s2;
        }

        step++;
        not_converged = std::abs(y2) > 1E-014;

        if(step > 1000){
            std::cout << "WARNING could not solve kirchhoffs slope" << std::endl;
            exit(99);
        }
    }
    return s2;
}

Kirchhoff_Piecewise_Erf::Kirchhoff_Piecewise_Erf(const Parameters &params) : Stem_flow_module(params) {

}

void Kirchhoff_Piecewise_Erf::Init() {

    // Obtain Weibull parameters from psi50 and psi88
    double x1 = 0.5;
    double x2 = 0.88;

    c = log(log(1. - x1)/log(1. - x2))/(log(-params.psi50_xylem) - log(-params.psi88_xylem));
    b = -params.psi50_xylem/std::pow(-log(1. - x1), (1./c));

    q95 = 0.95;
    psi_95 = -b * std::pow(-log(q95), 1.0/c);

    q05 = 0.05;
    psi_05 = -b * std::pow(-log(q05), 1.0/c);

    slope_low = FindKirchhoffSlope(params.psi50_xylem, psi_05, q05);
    slope_up = FindKirchhoffSlope(params.psi50_xylem, psi_95, q95);
}

double Kirchhoff_Piecewise_Erf::KirchhoffIntegral(double psi, double s) {
    double a = psi + std::exp(-s*s *(psi-params.psi50_xylem)*(psi-params.psi50_xylem))/std::sqrt(M_PI)/s;
    double b = (psi-params.psi50_xylem) * std::erf(s*(psi-params.psi50_xylem));
    return 0.5*(a +b);
}

double Kirchhoff_Piecewise_Erf::KirchhoffIntegralSplit(double psi) {
    if (psi < params.psi50_xylem){
        return KirchhoffIntegral(psi, slope_low);
    }
    else{
        return KirchhoffIntegral(psi, slope_up);
    }
}

double Kirchhoff_Piecewise_Erf::Get_Stem_flow(double psi_stem, double psi_leaf) {
    // Calculate hydrostatic pressure
    double psi_hydro = (rho_water * gravity * params.canopy_height) * PA_TO_MPA;

    // If leaf wand soil water potential are (almost) identical we avoid the divide by zero calcuation and
    // return zero water flow
    if (std::abs(psi_stem - psi_leaf) < 1E-12){
        return 0.0;
    }

    // Account for the effect of the hydrostatic pressure according to equation xxx
    double flow_psi_coeff = (1.0 - psi_hydro/(psi_stem - psi_leaf));

    // We avoid water flowing down the tree for now
    if (flow_psi_coeff < 0.0){
        return 0.0;
    }

    // Solving the conductivity integral
    double J_unit = KirchhoffIntegralSplit(psi_stem) - KirchhoffIntegralSplit(psi_leaf);

    return J_unit * params.k_xylem_sat * flow_psi_coeff * params.huber_value / (params.eta_LS * params.canopy_height);
}
