//
// Created by ppapastefanou on 4/8/24.
//

#include "stem_flow_model.h"
#include "amath.h"
#include "math.h"
#include <iostream>

Stem_flow_module::Stem_flow_module(const Parameters &params) :
        params(params){
}

Stem_flow_module::~Stem_flow_module() {
}


Linear_stem_flow::Linear_stem_flow(const Parameters &params) : Stem_flow_module(params) {
}

void Linear_stem_flow::Init(){

    // Obtain Weibull parameters from psi50 and psi88
    double x1 = 0.5;
    double x2 = 0.88;

    c = log(log(1. - x1)/log(1. - x2))/(log(-params.psi50_xylem) - log(-params.psi88_xylem));
    b = -params.psi50_xylem/std::pow(-log(1. - x1), (1./c));
}


double Linear_stem_flow::Get_Stem_flow(double psi_stem, double psi_leaf) {

    // The factor of two reflects the water uptake from the middle of the Stem to the canopy only
    // All units in MPa
    double DeltaP_LS = psi_stem - psi_leaf - (params.rho_water * params.grav * params.canopy_height / 2.0) * params.PaToMPa;

    // Prevent negative pressure differences to avoid letting the water flow down the tree
    if(DeltaP_LS < 0.0)
        DeltaP_LS = 0.0;

    // Calculate the average between leaf and stem water potential [MPa]
    double psi_avg = (psi_leaf + psi_stem) / 2.0;

    // Estimate the xylem conductance based on the satured xylem and the PLC of the xylem
    // [mol m-1 s-1 MPa-1]
    double k_xylem = params.k_xylem_sat * std::exp( -std::pow(-psi_avg / b, c));

    // Calculate the stem water flow J [mol m-2 s-1]
    // This is essentially Darcy's law
    return DeltaP_LS * k_xylem * params.huber_value / (params.eta_LS * params.canopy_height / 2.0);
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
    double psi_hydro = (params.rho_water * params.grav * params.canopy_height / 2.0) * params.PaToMPa;

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

    return J_unit * params.k_xylem_sat * flow_psi_coeff * params.huber_value / (params.eta_LS * params.canopy_height / 2.0);
}

double Kirchhoff_Piecewise_Erf::g_erf(double psi50, double slope, double psi_q, double q) {
    return 0.5* std::erf(slope * (psi_q - psi50)) + 0.5 - q;
}

double Kirchhoff_Piecewise_Erf::FindKirchhoffSlope(double psi50, double psi_q, double q) {
    double s0 = 0;
    double s1 = 0.5;

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
    psi_95 = -b*std::pow(-log(q95), 1.0/c);

    q05 = 0.05;
    psi_05 = -b*std::pow(-log(q05), 1.0/c);

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
    double psi_hydro = (params.rho_water * params.grav * params.canopy_height / 2.0) * params.PaToMPa;

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

    return J_unit * params.k_xylem_sat * flow_psi_coeff * params.huber_value / (params.eta_LS * params.canopy_height / 2.0);
}
