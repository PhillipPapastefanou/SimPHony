//
// Created by Phillip on 11.07.23.
//

#include "root_distribution_model.h"
#include <cmath>


Root_distribution_model::Root_distribution_model(const Parameters &parameters) {


    int nlayers = parameters.soil_layers.size();
    root_fractions.resize(nlayers);
    soil_depths_acc.resize(nlayers);

    // Accumulate soil depths
    soil_depths_acc[0] = parameters.soil_layers[0].depth;
    for (int i = 1; i < nlayers; ++i) {
        soil_depths_acc[i] = soil_depths_acc[i - 1] + parameters.soil_layers[i - 1].depth;
    }

    // Compute non-normalized root distributions according to jackson et al. 1996
    double rf_sum = 0.0;
    for (int i = 0; i < nlayers; ++i) {
        double rf  = std::pow(parameters.jackson_root_beta, 100.0 * soil_depths_acc[i]) -
                     std::pow(parameters.jackson_root_beta, 100.0 * (soil_depths_acc[i] + parameters.soil_layers[i].depth));
        rf_sum += rf;
        root_fractions[i] = rf;
    }
    // Ensure that all roots are distributed in the soil depth
    // that is, just a normalisation of root fractions
    for (int j = 0; j < nlayers; ++j) {
        root_fractions[j] *= 1.0 / rf_sum;
    }




}

std::vector<double> Root_distribution_model::Get_root_fractions() {
    return root_fractions;
}

std::vector<double> Root_distribution_model::Get_soil_layer_depth_acc() {
    return soil_depths_acc;
}
