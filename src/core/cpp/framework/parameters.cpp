#include "parameters.h"
#include "../modules/root_distribution_model.h"
#include <iostream>

Parameters::Parameters() {
}

Parameters::~Parameters() {

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


