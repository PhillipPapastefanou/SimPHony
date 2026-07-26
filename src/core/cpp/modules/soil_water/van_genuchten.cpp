//
// Created by Phillip on 03/09/2024.
//

#include "van_genuchten.h"
#include <iostream>

Van_Genuchten::Van_Genuchten(const Parameters &parameters, const Input &input, const Config& config) :
Soil_water_module(parameters, input, config) {

}

Van_Genuchten::~Van_Genuchten() noexcept {

}


void Van_Genuchten::CalculatePsiAndKs(int start_idx, int end_idx) {

    ParseTheta(start_idx, end_idx);

    vector<double> psi_row(nsoil);
    vector<double> k_row(nsoil);

    const int n = static_cast<int>(theta_2D.size());
    const int lo = start_idx > 0 ? start_idx : 0;
    const int hi = (end_idx < 0 || end_idx > n) ? n : end_idx;

    for (int i = lo; i < hi; ++i) {
        // The loop also reverse the layers to make the top layer be layer one.
        // Top layer must always be the layer 0
        vector<float > theta = theta_2D[i];

        for (int s = 0; s <  nsoil ; ++s) {
            const Soil_layer& layer = parameters.soil_layers[s];
            psi_row[s] = Psi_from_theta(theta[s], layer);
            k_row[s] = K_from_theta(theta[s], layer);
        }
        psi_soil_2D[i] = psi_row;
        ks_2D[i] = k_row;
    }
}

double Van_Genuchten::Psi_from_theta(double theta, const Soil_layer& layer) const {

    double theta_s = layer.theta_s;
    double theta_r = layer.theta_r;
    double pore_size_ind = layer.pore_size_ind;
    double psi_soil_sat = layer.psi_soil_sat;

    double n_vgn = pore_size_ind + 1;
    double m_vgn = pore_size_ind / n_vgn;

    double base = (theta - theta_r) / (theta_s - theta_r);

    // Absolutely no freakin water
    if (base < 0.0) {
        base = 0.000001;
    }

    // Fully saturated soil
    if (base > 1.0) {
        base = 1.0;
    }

    return psi_soil_sat * std::pow((std::pow(base, (-1.0 / m_vgn)) - 1.0), (1.0 / n_vgn));
}

double Van_Genuchten::K_from_theta(double theta, const Soil_layer& layer) const {

    double Ks = layer.k_soil_sat;
    double theta_s = layer.theta_s;
    double theta_r = layer.theta_r;
    double pore_size_ind = layer.pore_size_ind;

    double n_vgn = pore_size_ind + 1;
    double m_vgn = pore_size_ind / n_vgn;

    double base = (theta - theta_r) / (theta_s - theta_r);

    if (base < 0.0) {
        base = 0.000001;
    }

    if (base > 1.0) {
        base = 1.0;
    }

    return Ks * std::sqrt(base) * std::pow(1.0 - std::pow(1.0 - std::pow(base, (1.0 / m_vgn)), m_vgn), 2.0);
}


