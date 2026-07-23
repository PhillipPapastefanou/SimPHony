//
// Created by Phillip on 03/09/2024.
//

#include "saxton.h"
#include <iostream>

Saxton06::Saxton06(const Parameters &parameters, const Input &input, const Config &config) : Soil_water_module(
        parameters, input, config) {
}

void Saxton06::CalculatePsiAndKs(int start_idx, int end_idx) {

    ParseTheta(start_idx, end_idx);

    vector<double> psi_row(nsoil);
    vector<double> k_row(nsoil);

    Ks.resize(nsoil);
    sand_fracs.resize(nsoil);
    clay_fracs.resize(nsoil);
    orgmat_fracs.resize(nsoil);

    theta_t_33.resize(nsoil);
    theta_33.resize(nsoil);
    theta_s_33_t.resize(nsoil);
    theta_s_33.resize(nsoil);
    theta_s.resize(nsoil);
    theta_1500t.resize(nsoil);
    theta_1500.resize(nsoil);
    B.resize(nsoil);
    A.resize(nsoil);

    for (int is = 0; is < nsoil; ++is) {

        Soil_layer layer = parameters.soil_layers[is];

        Ks[is] = layer.k_soil_sat;
        sand_fracs[is] = layer.sand_fraction;
        clay_fracs[is] = layer.clay_fraction;
        orgmat_fracs[is] = layer.organic_matter_fraction;


        theta_t_33[is]      = calc_theta_33_t(is);
        theta_33[is]        = calc_theta_33(is);
        theta_s_33_t[is]    = calc_theta_s_33_t(is);
        theta_s_33[is]      = calc_theta_s_33(is);
        theta_s[is]         = calc_theta_s(is);

        theta_1500t[is]     = calc_theta_1500_t(is);
        theta_1500[is]      = calc_theta_1500(is);

        B[is]       = calc_B(is);
        A[is]       = calc_A(is, B[is]);
    }

    // Todo make an option to include the calculation of KS
    // for now we only describe the satuared ks
    //Ks      = calc_KS();


    // Loop through timesteps...
    const int n = static_cast<int>(theta_2D.size());
    const int lo = start_idx > 0 ? start_idx : 0;
    const int hi = (end_idx < 0 || end_idx > n) ? n : end_idx;
    for (int i = lo; i < hi; ++i) {

        vector<float > theta = theta_2D[i];

        // The loop also reverse the layers to make the top layer be layer one.
        // Top layer must always be the layer 0
        for (int s = 0; s <  nsoil ; ++s) {

            double base = theta[s]/ theta_s[s];
            if(base < 0.0){
                std::cout << "Invalid water content or theta_s paramter: ";
                std::cout << "Theta(t) is " << theta[s];
                std::cout << " and Theta_s is " << theta_s[s];
                std::cout << ". Exiting simulation..." << std::endl;
                exit(99);
            }

            // Fully saturated water
            if (base > 1.0){
                base = 1.0;
            }

            // Add a minus in fromt of A because water potentials are per definition negative
            psi_row[s] = - A[s] *  std::pow(base, -B[s]);

            // Multiply times g to get from kPA to m hydraulic head
            psi_row[s] *= parameters.constants.GRAVITY;

            k_row[s] = Ks[s] *  std::pow(base, 3.0 + 2.0 * B[s]);
        }

        psi_soil_2D[i] = psi_row;
        ks_2D[i] = k_row;

    }



}

double Saxton06::calc_theta_33_t(int si) {

    const double sf = sand_fracs[si];
    const double cf = clay_fracs[si];
    const double of = orgmat_fracs[si];
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction
    double d1 = -0.251 * sf + 0.195*cf + 0.011*ofp;
    double d2 = 0.006* sf * ofp - 0.027*cf*ofp + 0.452*cf*sf;
    return d1 + d2 + 0.299;
}

double Saxton06::calc_theta_33(int is) {

    const double th33t = theta_t_33[is];

    return th33t + 1.283*th33t*th33t - 0.374*th33t - 0.015;
}

double Saxton06::calc_theta_s_33_t(int is) {

    const double sf = sand_fracs[is];
    const double cf = clay_fracs[is];
    const double of = orgmat_fracs[is];
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction
    double d1 = 0.278 * sf + 0.034*cf + 0.022*ofp;
    double d2 = -0.018* sf * ofp - 0.027*cf*ofp - 0.584*cf*sf;
    return d1 + d2 + 0.078;
}

double Saxton06::calc_theta_s_33(int is) {

    const double th33mt = theta_s_33_t[is];

    return th33mt + 0.636*th33mt - 0.107;
}

double Saxton06::calc_theta_s(int is) {
    return theta_33[is] + theta_s_33[is] - 0.097 * sand_fracs[is] + 0.043;
}

double Saxton06::calc_theta_1500_t(int is) {

    const double sf = sand_fracs[is];
    const double cf = clay_fracs[is];
    const double of = orgmat_fracs[is];
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction

    double d1 = -0.024 * sf + 0.487*cf + 0.006*ofp;
    double d2 = 0.005* sf * ofp - 0.013*cf*ofp + 0.068*cf*sf;
    return d1 + d2 + 0.031;
}

double Saxton06::calc_theta_1500(int is) {

    const double th1500t = theta_1500t[is];
    return th1500t + 0.14*th1500t - 0.02;
}

double Saxton06::calc_B(int is) {
    double en = std::log(1500.0) - std::log(33.0);
    double den = std::log(theta_33[is]) - std::log(theta_1500[is]);
    return en/den;
}

double Saxton06::calc_A(int is, double B) {
    return std::exp(std::log(33.0) + B*std::log(theta_33[is]));
}


double Saxton06::calc_KS(int is) {
    const double L = 1.0/B[is];
    return 1930.0 * std::pow(theta_s[is] - theta_33[is], 3.0 - L);
}

Saxton06::~Saxton06() {

}
