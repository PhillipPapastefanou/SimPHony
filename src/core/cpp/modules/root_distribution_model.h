//
// Created by Phillip on 11.07.23.
//

#pragma once
#include "../framework/parameters.h"


class Root_distribution_model {

public :
    Root_distribution_model(const Parameters& parameters);

    std::vector<double> Get_root_fractions();
    std::vector<double> Get_soil_layer_depth_acc();
private:
    std::vector<double> root_fractions;
    std::vector<double> soil_depths_acc;
};

