//
// Created by Phillip on 15.03.24.
//

#include "input_hainich.h"
#include <iostream>
#include <algorithm>


Input_Hainich::Input_Hainich(const Parameters& parameters) : Input(parameters) {

}

void Input_Hainich::Read_N_Parse() {
    forcing_parser = std::make_unique<InputCollection>(forcing_file, true, ',');


    vector<int> forcing_indexes= {2,6,7,8,9 };
    string format = "%Y-%m-%d %H:%M:%S";
    forcing_parser->init_regular("datetime", format);
    vector<vector<float> > forcing_input = forcing_parser->get_data(forcing_indexes);
    DateTime t0_f = forcing_parser->dates.front();
    DateTime tz_f = forcing_parser->dates.back();

    //
    for (int i =0; i < forcing_parser->dates.size(); ++i) {
        double rad_d = forcing_input[i][1];
        this->rad.push_back(rad_d);
    }
    float max_rad =  *std::max_element(std::begin(rad), std::end(rad));

    // Slice forcing input according to indexes as we might have different offsets
    // for swc and other
    for (int i =0; i < forcing_parser->dates.size(); ++i) {
        dates.push_back(forcing_parser->dates[i]);
        this->vpd.push_back(forcing_input[i][0] * 1000.0);
        double anet = rad[i] /  max_rad * parameters.anet_max;
        this->anet.push_back(anet);

        vector<float> theta_run(forcing_input[i].begin() + 2, forcing_input[i].end());

        for (auto& value: theta_run) {
            value /= 100;
        }
        this->theta_per_layer.push_back(theta_run);
    }
}

void Input_Hainich::Add_Forcing_File(std::string file) {
    forcing_file = file;
}

void Input_Hainich::Add_Soilwater_File(std::string file) {

    // Nothing to be done here as this file is part of the forcing
}
