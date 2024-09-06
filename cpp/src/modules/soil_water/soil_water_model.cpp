//
// Created by Phillip on 11.07.23.
//

#include "soil_water_model.h"
#include <iostream>

Soil_water_module::Soil_water_module(const Parameters& parameters, const Input& input):
parameters(parameters), input_module(input){
}

Soil_water_module::~Soil_water_module() {
}

vector<vector<double> > Soil_water_module::Get_psi_head() {
    return psi_s_array;
}

vector<vector<double> > Soil_water_module::Get_ks() {
    return ks_array;
}