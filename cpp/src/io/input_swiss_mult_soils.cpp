//
// Created by Phillip on 11.07.23.
//

#include "input_swiss_mult_soils.h"
#include <numeric>
#include <iostream>

Input_Swiss_Multi_Soils::Input_Swiss_Multi_Soils(const Parameters& parameters): Input(parameters){

}

void Input_Swiss_Multi_Soils::Read_N_Parse() {

    theta_parser = std::make_unique<InputCollection>(theta_file, true, ',');
    forcing_parser = std::make_unique<InputCollection>(forcing_file, true, ',');

    vector<int> theta_indexes(8*3);
    std::iota(theta_indexes.begin(), theta_indexes.end(), 1);

    std::string format = "%Y-%m-%d %H:%M:%S";
    theta_parser->init_regular("dates", format);


    // Read all inputs
    vector<vector<float>> all_data  = theta_parser->get_data(theta_indexes);

    // The swiss inputs have 8 differnt sooil water measurements available
    // We selected one based on the input
    int water_content_location_index = parameters.soil_profile_index;

    // Each soil location has three soil layer depths
    vector<vector<float>> input_theta_per_layer(all_data.size());
    for (int i = 0; i < all_data.size(); ++i) {
        vector<float> sl(3);
        for (int s = 0; s < 3; ++s) {
            sl[s] = all_data[i][3 * water_content_location_index + s];

            // Convert from % to relative
            sl[s] /= 100.0;
        }

        input_theta_per_layer[i] = sl;
    }


    std::cout << input_theta_per_layer[0][0] << std::endl;

    vector<int> forcing_indexes= {2,8};
    format = "%Y-%m-%d %H:%M:%S";
    forcing_parser->init_regular("dt", format);
    vector<vector<float> > forcing_input = forcing_parser->get_data(forcing_indexes);

    DateTime t0_f = forcing_parser->dates.front();
    DateTime t0_w = theta_parser->dates.front();
    DateTime t0 = t0_f;

    if (t0_f < t0_w){
        t0 = t0_w;
    }
    long distance = -1000;
    int imin_forcing = -1;
    int imin_theta = -1;

    for (int i = 0; i < forcing_parser->dates.size(); ++i) {
        distance = forcing_parser->dates[i] - t0;
        if(distance == 0){
            imin_forcing = i;
            break;
        }
    }
    for (int i = 0; i < theta_parser->dates.size(); ++i) {
        distance = theta_parser->dates[i] - t0;
        if(distance == 0){
            imin_theta = i;
            break;
        }
    }

    if (imin_forcing == forcing_parser->dates.size() - 1){
        std::cout << "could not find index in data ";
        exit(99);
    }
    if (imin_theta == forcing_parser->dates.size() - 1){
        std::cout << "could not find index in data ";
        exit(99);
    }

    DateTime tz_f = forcing_parser->dates.back();
    DateTime tz_w = theta_parser->dates.back();
    DateTime tz = tz_f;

    if (tz_w < tz_f){
        tz = tz_w;
    }

    int imax_forcing = -1;
    int imax_theta = -1;

    for (int i = 0; i < forcing_parser->dates.size(); ++i) {
        distance = forcing_parser->dates[i] - tz;
        if(distance == 0){
            imax_forcing = i;
            break;
        }
    }

    for (int i = 0; i < theta_parser->dates.size(); ++i) {
        distance = theta_parser->dates[i] - tz;
        if(distance == 0){
            imax_theta = i;
            break;
        }
    }

    // Slice forcing input according to indexes as we might have different dates
    // for swc and other
    for (int i = imin_forcing; i < imax_forcing + 1; ++i) {
        dates.push_back(forcing_parser->dates[i]);
        this->vpd.push_back(forcing_input[i][1] * 1000.0);
        double rad_d = forcing_input[i][0];
        this->rad.push_back(rad_d);
        double anet = rad_d /  parameters.swdown_max * parameters.anet_max;
        this->anet.push_back(anet);
    }

    for (int i = imin_theta; i < imax_theta + 1; ++i) {
        this->theta_per_layer.push_back(input_theta_per_layer[i]);
    }

    if(rad.size() != theta_per_layer.size()){
        std::cout << "Thetas and forcing size does not match";
        double x= theta_per_layer.size();
        throw;
    }
}

void Input_Swiss_Multi_Soils::Add_Forcing_File(std::string file) {
    forcing_file = file;
}

void Input_Swiss_Multi_Soils::Add_Soilwater_File(std::string file) {
    theta_file = file;
}
