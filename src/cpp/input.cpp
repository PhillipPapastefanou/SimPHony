//
// Created by Phillip on 11.07.23.
//

#include "input.h"
#include <numeric>
#include <iostream>

Input::Input(string theta_file, string forcing_file, Parameters parameters):
        theta_parser(theta_file, true, ','),
        forcing_parser(forcing_file, true, ','),
        parameters(parameters){

}

void Input::Read_N_Parse() {

    vector<int> theta_indexes(11);
    std::iota(theta_indexes.begin(), theta_indexes.end(), 1);

    std::string format = "%Y-%m-%d %H:%M:%S";
    theta_parser.init("dt", format);
    // Directly write theta to memory
    theta_per_layer = theta_parser.get_data(theta_indexes);

    vector<int> forcing_indexes= {2,8};
    format = "%Y-%m-%d %H:%M:%S";
    forcing_parser.init("dt", format);
    vector<vector<float> > forcing_input = forcing_parser.get_data(forcing_indexes);


    DateTime t0_f = forcing_parser.dates.front();
    DateTime t0_w = theta_parser.dates.front();
    DateTime t0 = t0_f;

    if (t0_f < t0_w){
        t0 = t0_w;
    }
    long distance = -1000;
    int imin_forcing = -1;
    int imin_theta = -1;

    for (int i = 0; i < forcing_parser.dates.size(); ++i) {
        distance = forcing_parser.dates[i] - t0;
        if(distance == 0){
            imin_forcing = i;
            break;
        }
    }
    for (int i = 0; i < theta_parser.dates.size(); ++i) {
        distance = theta_parser.dates[i] - t0;
        if(distance == 0){
            imin_theta = i;
            break;
        }
    }

    if (imin_forcing == forcing_parser.dates.size() - 1){
        std::cout << "could not find index in data ";
        exit(99);
    }
    if (imin_theta == forcing_parser.dates.size() - 1){
        std::cout << "could not find index in data ";
        exit(99);
    }

    DateTime tz_f = forcing_parser.dates.back();
    DateTime tz_w = theta_parser.dates.back();
    DateTime tz = tz_f;

    if (tz_w < tz_f){
        tz = tz_w;
    }

    int imax_forcing = -1;
    int imax_theta = -1;

    for (int i = 0; i < forcing_parser.dates.size(); ++i) {
        distance = forcing_parser.dates[i] - tz;
        if(distance == 0){
            imax_forcing = i;
            break;
        }
    }

    for (int i = 0; i < theta_parser.dates.size(); ++i) {
        distance = theta_parser.dates[i] - tz;
        if(distance == 0){
            imax_theta = i;
            break;
        }
    }

    // Slice forcing input according to indexes as we might have different offsets
    // for swc and other
    for (int i = imin_forcing; i < imax_forcing + 1; ++i) {
        dates.push_back(forcing_parser.dates[i]);
        this->vpd.push_back(forcing_input[i][1] * 1000.0);
        double rad_d = forcing_input[i][0];
        this->rad.push_back(rad_d);
        double anet = rad_d / SWDOWN_MAX * ANET_MAX;
        this->anet.push_back(anet);
    }

    for (int i = imin_theta; i < imax_theta + 1; ++i) {
        // Nothing to do here
    }

    if(rad.size() != theta_per_layer.size()){
        std::cout << "Thetas and forcing size does not match";
        throw;
    }
}
