//
// Created by Phillip on 03/09/2024.
//

#include "campbell.h"
#include <iostream>

Campbell::Campbell(const Parameters &parameters, const Input &input, const Config& config) :
Soil_water_module(parameters, input, config)  {

}

Campbell::~Campbell() noexcept = default;


void Campbell::CalculatePsiAndKs(int start_idx, int end_idx) {

    ParseTheta(start_idx, end_idx);

    vector<double> psi_row(nsoil);
    vector<double> k_row(nsoil);

    const int n = static_cast<int>(theta_2D.size());
    const int lo = start_idx > 0 ? start_idx : 0;
    const int hi = (end_idx < 0 || end_idx > n) ? n : end_idx;

    for (int i = lo; i < hi; ++i) {

        // The loop also reverse the layers to make the top layer be layer one.
        // Top layer must always be the layer 0

        vector<float> theta = theta_2D[i];


        for (int s = 0; s < nsoil; ++s) {

            const Soil_layer &layer = parameters.soil_layers[s];

            // Preserve the original strict validation of observed forcing data here
            // (Psi_from_theta/K_from_theta themselves just clamp, since they are also
            // called by Soil_hydrology_richards with prognostically-evolving theta,
            // e.g. for numerical-derivative probes slightly below the current state,
            // where an exit(99) would be far too fragile).
            double base = theta[s] / layer.theta_s;
            if (base < 0.0) {
                std::cout << "Invalid water content or theta_s paramter: ";
                std::cout << "Theta(t) is " << theta[s];
                std::cout << " and Theta_s is " << layer.theta_s;
                std::cout << ". Exiting simulation..." << std::endl;
                exit(99);
            }

            psi_row[s] = Psi_from_theta(theta[s], layer);
            k_row[s] = K_from_theta(theta[s], layer);
        }

        psi_soil_2D[i] = psi_row;
        ks_2D[i] = k_row;
    }
}

double Campbell::Psi_from_theta(double theta, const Soil_layer& layer) const {

    double base = theta / layer.theta_s;
    if (base < 1.0e-6) {
        base = 1.0e-6;
    }
    if (base > 1.0) {
        base = 1.0;
    }

    return layer.psi_soil_sat * std::pow(base, -layer.camp_b);
}

double Campbell::K_from_theta(double theta, const Soil_layer& layer) const {

    double base = theta / layer.theta_s;
    if (base < 1.0e-6) {
        base = 1.0e-6;
    }
    if (base > 1.0) {
        base = 1.0;
    }

    // Hydraulic conductivity in m s-1
    return layer.k_soil_sat * std::pow(base, 3.0 + 2.0 * layer.camp_b);
}






