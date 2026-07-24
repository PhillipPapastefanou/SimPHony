//
// Created by Phillip on 11.07.23.
//

#include "soil_water_model.h"
#include <iostream>

Soil_water_module::Soil_water_module(const Parameters& parameters, const Input& input, const Config& config):
parameters(parameters), input_module(input), config(config){

    if(std::abs(parameters.wcont_sigma_deviation) > 1E-8 ){

        if (input_module.theta_sd_per_layer.empty()){
            std::cout << "Trying to use water content deviation without having water content data available." << std::endl;
            std::cout << "Consider setting parameter wcont_sigma_deviation to 0.0" << std::endl;
            exit(99);
        }

    }
}

Soil_water_module::~Soil_water_module() {
}

vector<vector<double> > Soil_water_module::Get_psi_soil_head() {
    return psi_soil_2D;
}

vector<vector<double> > Soil_water_module::Get_ks() {
    return ks_2D;
}

vector<vector<float> > Soil_water_module::Get_theta() {
    return theta_2D;
}

void Soil_water_module::ParseTheta(int start_idx, int end_idx) {

    const vector<vector<float> >& theta_raw_input = input_module.theta_per_layer;
    const int n = static_cast<int>(theta_raw_input.size());

    this->psi_soil_2D.resize(theta_raw_input.size());
    this->ks_2D.resize(theta_raw_input.size());

    number_of_layers_in_input = theta_raw_input[0].size();
    nsoil = parameters.soil_layers.size();

    if (config.swiss_soil_water_input_type.value == Swiss_soil_water_input_type::NLayersIndiv){
        number_of_layers_in_input/=8;
    }

    if(nsoil != number_of_layers_in_input){
        std::cout << "Number of soil layers does not match number of layers in input";
        exit(99);
    }

    theta_2D.resize(theta_raw_input.size());

    // Only the [start_idx, end_idx) sub-range gets populated -- see the
    // CalculatePsiAndKs() comment in soil_water_model.h. theta_2D/psi_soil_2D/
    // ks_2D still resize()d to the full series above so absolute indices
    // used elsewhere (Model::Run) stay valid; entries outside the requested
    // range are simply never read.
    const int lo = start_idx > 0 ? start_idx : 0;
    const int hi = (end_idx < 0 || end_idx > n) ? n : end_idx;

    // Loop through timesteps
    for (int i = lo; i < hi; ++i) {
        vector<float> theta_list;
        if (config.swiss_soil_water_input_type.value == Swiss_soil_water_input_type::NLayersIndiv) {
            theta_list.resize(3);
            for (int s = 0; s < 3; ++s) {
                theta_list[s] = theta_raw_input[i][s + parameters.soil_profile_index * 3];
            }
        } else {
            // Get water contents today for the number of soil layers
            theta_list = theta_raw_input[i];

            // Rescale all  water content in units of standard deviation
            // but only if this value is not zero
            if (std::abs(parameters.wcont_sigma_deviation) > 1E-8) {
                for (int s = 0; s < theta_list.size(); ++s) {
                    theta_list[s] += parameters.wcont_sigma_deviation * input_module.theta_sd_per_layer[i][s];
                }
            }
        }

        // Drought-stress "soil moisture" slider: constant multiplier on the
        // observed theta driver, applied before CalculatePsiAndKs() turns it
        // into psi_soil/k_soil -- so it feeds the actual water uptake, not
        // just the plotted series.
        for (float& t : theta_list) t *= parameters.theta_moisture_factor;

        theta_2D[i] = theta_list;
    }

}
