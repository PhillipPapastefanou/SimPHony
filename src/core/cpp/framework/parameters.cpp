#include "parameters.h"
#include "../modules/root_distribution_model.h"
#include <iostream>

Parameters::Parameters() {
}

Parameters::~Parameters() {

}

void Parameters::Set_soil_layers_field(double Soil_layer::* member, double value) {
    for (auto& layer : soil_layers) {
        layer.*member = value;
    }
}

void Parameters::Set_soil_k_sat_log10(double log10_k_soil_sat) {
    Set_soil_layers_field(&Soil_layer::k_soil_sat, std::pow(10.0, log10_k_soil_sat));
}

void Parameters::Set_soil_psi_soil_sat(double psi_soil_sat) {
    Set_soil_layers_field(&Soil_layer::psi_soil_sat, psi_soil_sat);
}

void Parameters::Set_soil_theta_s(double theta_s) {
    Set_soil_layers_field(&Soil_layer::theta_s, theta_s);
}

void Parameters::Set_soil_theta_r(double theta_r) {
    Set_soil_layers_field(&Soil_layer::theta_r, theta_r);
}

void Parameters::Set_soil_pore_size_ind(double pore_size_ind) {
    Set_soil_layers_field(&Soil_layer::pore_size_ind, pore_size_ind);
}

void Parameters::Set_soil_camp_b(double camp_b) {
    Set_soil_layers_field(&Soil_layer::camp_b, camp_b);
}

void Parameters::Set_soil_organic_matter_fraction(double organic_matter_fraction) {
    Set_soil_layers_field(&Soil_layer::organic_matter_fraction, organic_matter_fraction);
}

void Parameters::Set_soil_sand_fraction(double sand_fraction) {
    Set_soil_layers_field(&Soil_layer::sand_fraction, sand_fraction);
}

void Parameters::Set_soil_clay_fraction(double clay_fraction) {
    Set_soil_layers_field(&Soil_layer::clay_fraction, clay_fraction);
}

void Parameters::Set_derived() {

    size_t n_soil_layers =  soil_layers.size();

    Root_distribution_model root_model(*this);
    // Rooting fraction per laye [-]
    std::vector<double> root_fraction_player = root_model.Get_root_fractions();
    // Soil layer depths accumulated
    std::vector<double> soil_layer_depth_acc  = root_model.Get_soil_layer_depth_acc();

    for (int i = 0; i < n_soil_layers; ++i) {
        soil_layers[i].root_fraction = root_fraction_player[i];
    }

}


