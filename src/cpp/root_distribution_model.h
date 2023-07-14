//
// Created by Phillip on 11.07.23.
//

#pragma once
#include "parameters.h"


class Root_distribution_model {

public :
    Root_distribution_model(const Parameters& parameters);

    std::vector<double> Get_rooting_fractions();
private:
    std::vector<double> root_fractions;
};

