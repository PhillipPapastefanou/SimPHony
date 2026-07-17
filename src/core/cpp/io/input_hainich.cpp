//
// Created by Phillip on 15.03.24.
//

#include "input_hainich.h"
#include <iostream>
#include <algorithm>


Input_Hainich::Input_Hainich(const Config& config) : Input(config) {

}
void Input_Hainich::Set_Forcing_Data(const vector<long>& timestamps, 
                                     const vector<float>& vpd_in, 
                                     const vector<float>& rad_in, 
                                     const vector<vector<float>>& theta_in) {
    
    this->dates.clear();
    this->vpd.clear();
    this->sw_rad.clear();
    this->theta_per_layer.clear();
    this->temp_air.clear();

    const double KTOC = 273.15;


    for (size_t i = 0; i < timestamps.size(); ++i) {


        //DateTime dt(timestamps[i]); 
        //this->dates.push_back(dt);


        this->vpd.push_back(vpd_in[i] * 1000.0);
        this->sw_rad.push_back(rad_in[i]);

        vector<float> theta_run = theta_in[i];
        for (auto& value : theta_run) {
            value /= 100.0f;
        }
        this->theta_per_layer.push_back(theta_run);


        double air_temp_kelv = 300;
        this->temp_air.push_back(air_temp_kelv - KTOC );
    }
}


void Input_Hainich::Set_Forcing_Data_Fast(uintptr_t ts_ptr, uintptr_t vpd_ptr,
                                           uintptr_t rad_ptr, uintptr_t theta_ptr,
                                           int n, int n_layers) {
    auto* ts   = reinterpret_cast<int32_t*>(ts_ptr);
    auto* vpd  = reinterpret_cast<float*>(vpd_ptr);
    auto* rad  = reinterpret_cast<float*>(rad_ptr);
    auto* theta= reinterpret_cast<float*>(theta_ptr);

    dates.clear(); 
    this->vpd.clear(); 
    sw_rad.clear();
    theta_per_layer.clear();
    temp_air.clear();

    for (int i = 0; i < n; ++i) {
        //dates.emplace_back(ts[i]);
        this->vpd.push_back(vpd[i] * 1000.0f);
        sw_rad.push_back(rad[i]);
        vector<float> row(theta + i*n_layers, theta + (i+1)*n_layers);
        for (auto& v : row) v /= 100.0f;
        theta_per_layer.push_back(std::move(row));
        temp_air.push_back(300.0 - 273.15);
    }
}
void Input_Hainich::Set_Forcing_Data_Blob(uintptr_t blob_ptr, int n, int n_layers) {
    auto* base  = reinterpret_cast<uint8_t*>(blob_ptr);
    auto* ts    = reinterpret_cast<int32_t*>(base);
    auto* vpd   = reinterpret_cast<float*>(base + n * 4);
    auto* rad   = reinterpret_cast<float*>(base + n * 8);
    auto* temp  = reinterpret_cast<float*>(base + n * 12);
    auto* theta = reinterpret_cast<float*>(base + n * 16); 

    dates.clear(); this->vpd.clear(); sw_rad.clear();
    theta_per_layer.clear(); temp_air.clear();

    for (int i = 0; i < n; ++i) {
        dates.push_back(DateTime(static_cast<time_t>(ts[i])));
        this->vpd.push_back(vpd[i] * 1000.0f);
        sw_rad.push_back(rad[i]);
        temp_air.push_back(temp[i]);   // CSV is already deg C, no conversion needed
        vector<float> row(theta + i*n_layers, theta + (i+1)*n_layers);
        for (auto& v : row) v /= 100.0f;
        theta_per_layer.push_back(std::move(row));
    }
}

void Input_Hainich::Read_N_Parse() {

    forcing_parser = std::make_unique<InputCollection>(config.forcing_file.value, true, ',');

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

    // Slice forcing input according to indexes as we might have different dates
    // for swc and other
    for (int i =0; i < forcing_parser->dates.size(); ++i) {
        dates.push_back(forcing_parser->dates[i]);
        this->vpd.push_back(forcing_input[i][0] * 1000.0);
        this->sw_rad.push_back(rad[i]);

        vector<float> theta_run(forcing_input[i].begin() + 2, forcing_input[i].end());

        for (auto& value: theta_run) {
            value /= 100;
        }
        this->theta_per_layer.push_back(theta_run);
    }
}