//
// Created by Phillip on 14.03.24.
//
#pragma once
#include "../framework/parameters.h"
#include "../framework/date_time.h"
#include "../framework/config.h"
#include <string>
#include <vector>

class Input {

public:
    Input(const Config& config);
    virtual ~Input();
    virtual void Read_N_Parse() = 0;
    // Date that are going to be simulated
    std::vector<DateTime> dates;
    /// Shortwave radiation [-]
    std::vector<float> sw_rad;
    /// Surface air temperature [Degree Celcius]
    std::vector<float> temp_air;
    /// Vapour pressure deficit [Pa]
    std::vector<float> vpd;
    /// Volumetric water content [m3 m-3]
    std::vector<std::vector<float>> theta_per_layer;
    // Volumetric water standard deviation [m3 m-3]
    // Currently only available for the Swiss sites
    std::vector<std::vector<float>> theta_sd_per_layer;
    /// Precipitation rate [kg m-2 s-1], i.e. mm water per second. Only used by the
    /// prognostic soil hydrology option (Soil_hydrology_richards); empty/unused otherwise.
    std::vector<float> precip;
protected:
    const Config& config;
};


