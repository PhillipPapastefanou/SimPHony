//
// Created by Phillip on 03/09/2024.
//

#include "campbell.h"
#include <iostream>

Campbell::Campbell(const Parameters &parameters, const Input &input) :
Soil_water_module(parameters, input)  {

}

Campbell::~Campbell() noexcept {

}


void Campbell::CalculatePsiAndKs() {

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

            const Soil_layer& layer = parameters.soil_layers[s];

            double Ks = layer.k_soil_sat;
            double camp_b = layer.camp_b;
            double theta_s = layer.theta_s;
            double psi_soil_sat = layer.psi_soil_sat;


            double base = theta_list[s] / theta_s;
            if(base < 0.0){
                std::cout << "Invalid water content or theta_s paramter: ";
                std::cout << "Theta(t) is " << theta_list[s];
                std::cout << " and Theta_s is " << theta_s;
                std::cout << ". Exiting simulation..." << std::endl;
                exit(99);
            }

            // Fully saturated water
            if (base > 1.0){
                base = 1.0;
            }

            psi_row[s] = psi_soil_sat * std::pow(base, -camp_b);

            // Hydraulic conductivity in m s-1
            k_row[s] = Ks *  std::pow(base, 3.0 + 2.0 * camp_b);
        }

        psi_soil_2D[i] = psi_row;
        ks_2D[i] = k_row;
    }
}





