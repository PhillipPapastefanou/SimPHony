//
// Created by Phillip on 11.07.23.
//

#include "root_distribution_model.h"
#include <cmath>
#include <iostream>

Root_distribution_model::Root_distribution_model(const Parameters &parameters) {

    root_fractions.resize(parameters.nsoil);

    double rf_sum = 0.0;


    for (int i = 0; i < parameters.nsoil; ++i) {

        double x = parameters.min_soil_layer_depth + i * parameters.layer_depth;
        double rf  = std::pow(parameters.jackson_root_beta, 100.0 * x) -
                     std::pow(parameters.jackson_root_beta, 100.0 * (x + parameters.layer_depth));

        rf_sum += rf;
        root_fractions[i] = rf;
    }


    for (int j = 0; j < parameters.nsoil; ++j) {
        root_fractions[j] *= 1.0/rf_sum;
    }



}

std::vector<double> Root_distribution_model::Get_rooting_fractions() {
    return root_fractions;
}
