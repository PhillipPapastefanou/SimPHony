//
// Created by Phillip on 11.07.23.
//

#include "soil_water_model.h"

Soil_water_module::Soil_water_module(const Parameters& parameters, const Input& input):
parameters(parameters), input_module(input){

}

Soil_water_module::~Soil_water_module() {

}

void Campbell_Water_Uptake::CalculatePsiAndKs() {

    double b = parameters.camp_b;
    double ksat = parameters.k_soil_sat;
    double theta_s = parameters.theta_s;
    double psi_s_ref = parameters.camp_psi_soil_ref;
    int nsoil = parameters.nsoil;

    vector<vector<float> > theta_array = input_module.theta_per_layer;
    this->psi_s_array.resize(theta_array.size());
    this->ks_array.resize(theta_array.size());


    for (int i = 0; i < theta_array.size(); ++i) {

        vector<float> theta_list = theta_array[i];


        // Resize all theta water contents of each layers

        for (float& wcont : theta_list) {
            wcont *= parameters.theta_emp_multiplier;
        }


        int navail_soil_in_data = theta_list.size();

        vector<double> psi_row(nsoil);
        vector<double> k_row(nsoil);


        int s_h = 0;
        // The loop also reverse the layers to make the top layer be layer one.
        for (int s = navail_soil_in_data - 1; s > navail_soil_in_data - 1 - nsoil  ; --s) {

            psi_row[s_h] = psi_s_ref *  std::pow(theta_list[s]/theta_s, -b);
            k_row[s_h] = ksat *  std::pow(theta_list[s]/theta_s, 2.0 + 3.0*b);

            s_h++;
        }

        psi_s_array[i] = psi_row;
        ks_array[i] = k_row;

    }
}

vector<vector<double> > Campbell_Water_Uptake::Get_psi() {
    return psi_s_array;
}

vector<vector<double> > Campbell_Water_Uptake::Get_ks() {
    return ks_array;
}

Campbell_Water_Uptake::Campbell_Water_Uptake(const Parameters& parameters, const Input& input): Soil_water_module(parameters,input)
{

}

Campbell_Water_Uptake::~Campbell_Water_Uptake() {

}
