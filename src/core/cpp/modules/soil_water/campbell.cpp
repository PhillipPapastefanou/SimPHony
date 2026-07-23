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

            double Ks = layer.k_soil_sat;
            double camp_b = layer.camp_b;
            double theta_s = layer.theta_s;
            double psi_soil_sat = layer.psi_soil_sat;

            double base = theta[s] / theta_s;
            if (base < 0.0) {
                std::cout << "Invalid water content or theta_s paramter: ";
                std::cout << "Theta(t) is " << theta[s];
                std::cout << " and Theta_s is " << theta_s;
                std::cout << ". Exiting simulation..." << std::endl;
                exit(99);
            }

            // Fully saturated water
            if (base > 1.0) {
                base = 1.0;
            }

            psi_row[s] = psi_soil_sat * std::pow(base, -camp_b);

            // Hydraulic conductivity in m s-1
            k_row[s] = Ks * std::pow(base, 3.0 + 2.0 * camp_b);
        }

        psi_soil_2D[i] = psi_row;
        ks_2D[i] = k_row;
    }
}






