//
// Created by Phillip on 14.03.24.
//
#pragma once
#include "parameters.h"
#include "date_time.h"
#include <string>
#include <vector>

class Input {

public:
    Input(Parameters parameters);

    virtual void Add_Forcing_File(std::string file) = 0;
    virtual void Add_Soilwater_File(std::string file) = 0;
    virtual void Read_N_Parse() = 0;

    /// Vapour pressure deficit [Pa]
    std::vector<float> anet;
    /// Vapour pressure deficit [Pa]
    std::vector<float> vpd;
    /// Volumetric water content [m3 m-3]
    std::vector<std::vector<float>> theta_per_layer;
    // Date that are going to be simulated
    std::vector<DateTime> dates;

protected:
    Parameters parameters;
};


