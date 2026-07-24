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

    // start_idx/end_idx (in units of the input forcing's own timesteps, end
    // exclusive) restrict the precalculation to a sub-range of the loaded
    // forcing series instead of all of it -- Model::Run() only ever reads
    // indices within its own [begin, end] simulation window, so anything
    // outside that range is otherwise wasted work. Defaults (0, -1) mean
    // "the whole series", preserving prior behavior for existing callers.
    virtual void CalculatePsiAndKs(int start_idx = 0, int end_idx = -1) = 0;
    /// Matric potential of each soil layer expressed as hydraulic head [m]
    vector<vector<double> > Get_psi_soil_head();
    /// Hydraulic conductivity per soil layer [m s-1]
    vector<vector<double> > Get_ks();
    /// Volumetric water content per soil layer [m3 m-3]
    vector<vector<float> > Get_theta();

protected:

    void ParseTheta(int start_idx = 0, int end_idx = -1);

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



