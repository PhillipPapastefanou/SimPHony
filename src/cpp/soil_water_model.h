//
// Created by Phillip on 11.07.23.
//

#include "parameters.h"
#include "input.h"
#include <vector>

#pragma once

using std::vector;

class Soil_water_module {

public:
    Soil_water_module(const Parameters& parameters, const Input& input);

    virtual ~Soil_water_module();

    virtual void CalculatePsiAndKs() = 0;

    virtual vector<vector<double> > Get_psi() = 0;
    virtual vector<vector<double> > Get_ks() = 0;


protected:
    const Parameters& parameters;
    const Input& input_module;

    vector<vector<double> > ks_array;
    vector<vector<double> > psi_s_array;

};

class Campbell_Water_Uptake: public Soil_water_module {
public:
    Campbell_Water_Uptake(const Parameters& parameters, const Input& input);
    ~Campbell_Water_Uptake();
    void CalculatePsiAndKs() override;

    vector<vector<double>> Get_psi() override;
    vector<vector<double>> Get_ks() override;

};


//class VanGenuchten_Water_Uptake:


