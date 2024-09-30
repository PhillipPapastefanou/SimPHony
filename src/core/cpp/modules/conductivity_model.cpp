//
// Created by Phillip on 29.04.24.
//

#include "conductivity_model.h"
#include <iostream>

Condutivity_Module::Condutivity_Module(const Parameters& params):
        psi_50(params.psi50_xylem), psi_88(params.psi88_xylem) {


    if (params.psi50_xylem < params.psi88_xylem){
        std::cout << "Error: psi50(" << params.psi50_xylem <<") is smaller  than psi88(";
        std::cout << params.psi88_xylem << ")! This is physically impossible and must be fixed!" << std::endl;
        exit(99);
    }

}

Condutivity_Module::~Condutivity_Module() {

}

void WeibullCDF::Init() {

    // Obtain Weibull parameters from psi50 and psi88
    double x1 = 0.5;
    double x2 = 0.88;

    c = log(log(1. - x1)/log(1. - x2))/(log(-psi_50) - log(-psi_88));
    b = -psi_50/std::pow(-log(1. - x1), (1./c));
}

double WeibullCDF::Get_fraction(double psi) {
    return std::exp(-std::pow(-psi / b, c));
}

WeibullCDF::WeibullCDF(const Parameters &parameters) : Condutivity_Module(parameters) {

}

Logit::Logit(const Parameters &parameters) : Condutivity_Module(parameters) {

}

void Logit::Init() {
    slope = std::log(22.0/3.0) /  (psi_50 - psi_88);
}

double Logit::Get_fraction(double psi) {
    return 1.0 / (1.0 + std::exp(-1.0 * slope *(psi - psi_50)));
}
