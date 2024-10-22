//
// Created by Phillip on 08/10/2024.
//

#include "input_swiss_indiv_variation.h"
#include <numeric>
#include <iostream>

Input_Swiss_Indiv_Variation::Input_Swiss_Indiv_Variation(): Input(){

}



void Input_Swiss_Indiv_Variation::Add_Forcing_File(std::string file) {
    forcing_file = file;
}

void Input_Swiss_Indiv_Variation::Add_Soilwater_File(std::string file) {
    theta_file = file;
}

void Input_Swiss_Indiv_Variation::Read_N_Parse() {

    theta_parser = std::make_unique<InputCollection>(theta_file, true, ',');
    forcing_parser = std::make_unique<InputCollection>(forcing_file, true, ',');

    vector<string> soil_water_indexes({"theta_10cm","theta_std_10cm",
                                       "theta_40cm","theta_std_40cm",
                                       "theta_80cm","theta_std_80cm"});

    std::string format = "%Y-%m-%d %H:%M:%S";
    theta_parser->init_regular("dates", format);

    // Read all soil water inpults
    vector<vector<float> > soil_water_input = theta_parser->get_data(soil_water_indexes);

    vector<int> forcing_indexes= {2,8};
    format = "%Y-%m-%d %H:%M:%S";
    forcing_parser->init_regular("dt", format);
    vector<vector<float> > forcing_input = forcing_parser->get_data(forcing_indexes);

    DateTime t0_f = forcing_parser->dates.front();
    DateTime t0_w = theta_parser->dates.front();

    DateTime t_end_f = forcing_parser->dates.back();
    DateTime t_end_w = theta_parser->dates.back();

    if(t0_f != t0_w){
        std::cout << "Starting dates of forcing and soil_water input do not match" << std:: endl;
        exit(99);
    }

    if(t_end_f != t_end_w){
        std::cout << "End dates of forcing and soil_water input do not match" << std:: endl;
        exit(99);
    }

    if(forcing_parser->dates.size() !=  theta_parser->dates.size()){
        std::cout << "Number of dates of forcing and soil_water input do not match" << std:: endl;
        exit(99);
    }

    // Each soil location has three soil layer depths and std
    for (int i = 0; i < soil_water_input.size(); ++i) {
        vector<float> sl(3);
        vector<float> sl_std(3);
        for (int s = 0; s < 3; ++s) {
            sl[s] = soil_water_input[i][2*s];
            sl_std[s] = soil_water_input[i][2*s + 1];
        }
        theta_per_layer.push_back(sl);
        theta_sd_per_layer.push_back(sl_std);
    }

    // Slice forcing input according to indexes as we might have different dates
    // for swc and other
    for (int i = 0; i < forcing_input.size(); ++i) {
        dates.push_back(forcing_parser->dates[i]);
        this->vpd.push_back(forcing_input[i][1] * 1000.0);
        double rad_d = forcing_input[i][0];
        this->sw_rad.push_back(rad_d);
    }
}

