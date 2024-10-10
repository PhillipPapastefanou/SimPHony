//
// Created by Phillip on 11.07.23.
//

#include "soil_water_model.h"
#include <iostream>

Soil_water_module::Soil_water_module(const Parameters& parameters, const Input& input):
parameters(parameters), input_module(input){

    if(std::abs(parameters.wcont_sigma_deviation) > 1E-8 ){

        if (input_module.theta_sd.empty()){
            std::cout << "Trying to use water content deviation without having water content data available." << std::endl;
            std::cout << "Consider setting parameter wcont_sigma_deviation to 0.0" << std::endl;
            exit(99);

        }

    }
}

Soil_water_module::~Soil_water_module() {
}

vector<vector<double> > Soil_water_module::Get_psi_soil_head() {
    return psi_soil_2D;
}

vector<vector<double> > Soil_water_module::Get_ks() {
    return ks_2D;
}