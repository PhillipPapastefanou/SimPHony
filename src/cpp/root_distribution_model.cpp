//
// Created by Phillip on 11.07.23.
//

#include "root_distribution_model.h"
#include <cmath>
#include <iostream>

Root_distribution_model::Root_distribution_model(const Parameters &parameters) {

    int nlayers = parameters.soil_depths.size();
    root_fractions.resize(nlayers);

    std::vector<double> soil_depths_acc(nlayers);
    soil_depths_acc[0] = parameters.soil_depths[0];

    for (int i = 1; i < nlayers; ++i) {
        soil_depths_acc[i] = soil_depths_acc[i - 1] + parameters.soil_depths[i - 1];
    }

    double rf_sum = 0.0;
    for (int i = 0; i < nlayers; ++i) {
        double rf  = std::pow(parameters.jackson_root_beta, 100.0 * soil_depths_acc[i]) -
                     std::pow(parameters.jackson_root_beta, 100.0 * (soil_depths_acc[i] + parameters.soil_depths[i]));
        rf_sum += rf;
        root_fractions[i] = rf;
    }

    // Ensure that all roots are distributed in the soil depth
    // Normalisation of root fractions
    for (int j = 0; j < nlayers; ++j) {
        root_fractions[j] *= 1.0/rf_sum;
    }



}

std::vector<double> Root_distribution_model::Get_rooting_fractions() {
    return root_fractions;
}
