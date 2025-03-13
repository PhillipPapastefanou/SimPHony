//
// Created by Phillip on 11.07.23.
//

#include "input_swiss_mult_soils.h"
#include <numeric>
#include <iostream>

Input_Swiss_Multi_Soils::Input_Swiss_Multi_Soils(const Config& config): Input(config){

}

void Input_Swiss_Multi_Soils::Read_N_Parse() {

    const std::string format = "%Y-%m-%d %H:%M:%S";

    theta_parser = std::make_unique<InputCollection>(config.soilwater_file.value, true, ',');
    forcing_parser = std::make_unique<InputCollection>(config.forcing_file.value, true, ',');

    // Read all soil water inputs
    vector<vector<float> > soil_water_input;

    switch (config.swiss_soil_water_input_type.value) {
        case Swiss_soil_water_input_type::NLayersMean:
            break;
        case Swiss_soil_water_input_type::NLayersMeanOneStd:{
            vector<string> soil_water_indexes({"v10_adapted","v40_adapted","v80_adapted","sd"});
            theta_parser->init_regular("date", format);
            soil_water_input = theta_parser->get_data(soil_water_indexes);
            break;
        }
        case Swiss_soil_water_input_type::NLayersMeanNStd:{
            vector<string> soil_water_indexes({"theta_10cm","theta_std_10cm",
                                               "theta_40cm","theta_std_40cm",
                                               "theta_80cm","theta_std_80cm"});
            theta_parser->init_regular("dates", format);
            soil_water_input = theta_parser->get_data(soil_water_indexes);
            break;
        }

        case Swiss_soil_water_input_type::NLayersIndiv:{
            vector<int> theta_indexes(8*3);
            std::iota(theta_indexes.begin(), theta_indexes.end(), 1);
            theta_parser->init_regular("dates", format);
            soil_water_input = theta_parser->get_data(theta_indexes);
            break;
        }
        default:{
            std::cout << "Invalid soil water input type specified";
            exit(99);
            break;
        }

    }

    vector<int> forcing_indexes= {2,8,3};
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

    const double KTOC = 273.15;
    // Read in forcing
    for (int i = 0; i < forcing_input.size(); ++i) {
        dates.push_back(forcing_parser->dates[i]);
        this->vpd.push_back(forcing_input[i][1] * 1000.0);
        double rad_d = forcing_input[i][0];
        this->sw_rad.push_back(rad_d);
        double air_temp_kelv = forcing_input[i][2];
        this->temp_air.push_back(air_temp_kelv - KTOC );
    }


    switch (config.swiss_soil_water_input_type.value) {

        case Swiss_soil_water_input_type::NLayersMean:
            break;
        case Swiss_soil_water_input_type::NLayersMeanOneStd:{

            // Each soil location has three soil layer depths
            for (int i = 0; i < soil_water_input.size(); ++i) {
                vector<float> sl(3);
                for (int s = 0; s < 3; ++s) {
                    sl[s] = soil_water_input[i][s];
                }
                theta_per_layer.push_back(sl);

                // Copy std to each layer
                // We assume that the std applies ot each of the three layers
                vector<float> sl_std(3);
                for (int s = 0; s < 3; ++s) {
                    sl_std[s] = soil_water_input[i][3];
                }
                theta_sd_per_layer.push_back(sl_std);
            }
            break;
        }

        case Swiss_soil_water_input_type::NLayersMeanNStd:{
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
            break;
        }
        case Swiss_soil_water_input_type::NLayersIndiv:{
            // Copy all data into memory
            for (int i = 0; i < soil_water_input.size(); ++i) {
                vector<float> sl(8*3);
                for (int s = 0; s < 8*3; ++s) {
                    sl[s] = soil_water_input[i][s];
                }
                theta_per_layer.push_back(sl);
            }
            break;
        }
    }

    // Test input water to be below 0 and 1
    for (int i = 0; i < theta_per_layer.size(); ++i) {
        int ymax = theta_per_layer.front().size();
        for (int s = 0; s < ymax; ++s) {
            double value =  theta_per_layer[i][s];
            if ((value < 0.0) || (value > 1.0)){
                std::cout << "Invalid value for water content received: ";
                std::cout << std::to_string(value) << std::endl;
                std::cout << "Exiting..." << std::endl;
                exit(99);
            }
        }
    }

}