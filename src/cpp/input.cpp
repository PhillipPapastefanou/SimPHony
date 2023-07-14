//
// Created by Phillip on 11.07.23.
//

#include "input.h"
#include <numeric>
#include <iostream>

Input::Input(string theta_file, string forcing_file):
theta_per_layer_reader(theta_file, true, ','),
forcing_reader(forcing_file, true, ','){



}

void Input::Read_N_Parse() {

    vector<int> theta_indexes(11);

    std::iota(theta_indexes.begin(), theta_indexes.end(), 1);

    theta_per_layer = theta_per_layer_reader.Get<float>(theta_indexes);

    vector<int> forcing_indexes= {2,8};

    vector<vector<float> > forcing_input = forcing_reader.Get<float>(forcing_indexes);





    //Slice forcing input according to indexes as we might have differnt data for swc and other
    int begin = 7248;
    int end  = 17473;

    for (int i =begin; i < end; ++i) {

        this->vpd.push_back(forcing_input[i][1] * 1000.0);
        double rad_d = forcing_input[i][0];
        this->rad.push_back(rad_d);
        double anet = rad_d / SWDOWN_MAX * ANET_MAX;

        this->anet.push_back(anet);
    }


    if(rad.size() != theta_per_layer.size()){
        std::cout << "Theats and forcing size does not match";
        throw;
    }



    

}
