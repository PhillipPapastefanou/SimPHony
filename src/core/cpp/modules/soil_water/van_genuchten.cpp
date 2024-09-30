//
// Created by Phillip on 03/09/2024.
//

#include "van_genuchten.h"
#include <iostream>

Van_Genuchten::Van_Genuchten(const Parameters &parameters, const Input &input) : Soil_water_module(parameters, input) {

}

Van_Genuchten::~Van_Genuchten() noexcept {

}


void Van_Genuchten::CalculatePsiAndKs() {

    vector<vector<float> > theta_array = input_module.theta_per_layer;

    this->psi_soil_2D.resize(theta_array.size());
    this->ks_2D.resize(theta_array.size());

    int number_of_layers_in_input = theta_array[0].size();
    int nsoil = parameters.soil_layers.size();

    if(nsoil != number_of_layers_in_input){
        std::cout << "Number of soil layers does not match number of layers in input";
        exit(99);
    }


    for (int i = 0; i < theta_array.size(); ++i) {

        vector<float> theta_list = theta_array[i];

        // Resize all theta water contents of each layers
        for (float &wcont: theta_list) {
            wcont *= parameters.theta_emp_multiplier;
        }


        vector<double> psi_row(nsoil);
        vector<double> k_row(nsoil);

        // The loop also reverse the layers to make the top layer be layer one.
        // Top layer must always be the layer 0
        for (int s = 0; s <  nsoil ; ++s) {

            Soil_layer layer = parameters.soil_layers[s];

            double Ks = layer.k_soil_sat;
            double theta_s = layer.theta_s;
            double theta_r = layer.theta_r;
            double pore_size_ind = layer.pore_size_ind;
            double psi_soil_sat = layer.psi_soil_sat;

            double n_vgn = pore_size_ind + 1;
            double m_vgn = pore_size_ind / n_vgn;

            double base = (theta_list[s] - theta_r) / (theta_s  - theta_r);

            // Absoultely no freakin water
            if(base < 0.0){
                base = 0.000001;
            }

            // Fully saturated soil
            if (base > 1.0){
                base = 1.0;
            }

            psi_row[s] = psi_soil_sat * std::pow((std::pow(base, (-1.0 / m_vgn)) - 1.0), (1.0 / n_vgn));

            k_row[s] = Ks * std::sqrt(base) * std::pow(1.0 - std::pow(1.0 - std::pow(base, (1.0 / m_vgn)), m_vgn), 2.0);
        }

        psi_soil_2D[i] = psi_row;
        ks_2D[i] = k_row;

    }



}
