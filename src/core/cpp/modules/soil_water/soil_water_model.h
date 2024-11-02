//
// Created by Phillip on 11.07.23.
//
#pragma once
#include "../../framework/parameters.h"
#include "../../io/input.h"
#include <vector>



using std::vector;



class Soil_water_module {

public:
    Soil_water_module(const Parameters& parameters, const Input& input, const Config& config);
    virtual ~Soil_water_module();

    virtual void CalculatePsiAndKs() = 0;
    /// Matric potential of each soil layer expressed as hydraulic head [m]
    vector<vector<double> > Get_psi_soil_head();
    /// Hydraulic conductivity per soil layer [m s-1]
    vector<vector<double> > Get_ks();

protected:

    void ParseTheta();

    const Parameters& parameters;
    const Input& input_module;
    const Config& config;

    /// Hydraulic conductivity per soil layer [m s-1]
    vector<vector<double> > ks_2D;
    /// Matric potential of each soil layer [m]
    vector<vector<double> > psi_soil_2D;

    int nsoil;
    int number_of_layers_in_input;
    std::vector< std::vector< float> > theta_2D;

};



