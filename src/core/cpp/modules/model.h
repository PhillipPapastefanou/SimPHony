#pragma once
#include "../framework/parameters.h"
#include "../io/input.h"
#include <vector>
#include "soil_water/soil_water_model.h"
#include "../auxil/solvers.h"
#include "../io/output.h"
#include "model.h"
#include "../framework/date_time.h"
#include "stem_flow_model.h"
#include "../framework/water_potential_solver.h"

class Model{

public:
    Model(Parameters& parameters, Input& input, Config& config);
    void Set_derived_parameters();
    void Set_initial_conditions(double psi_leaf_zero, double psi_soil_zero);
    void Run(DateTime begin, DateTime end);
    const Output& Get_output() const;
    void Clear_output();
private:
    // Input references
    const Parameters& params;
    Input& input_module;
    const Config& config;

    // vapor pressure deficit [Pa]
    vector<float> input_vpd;
    // Shortwave downward radiation [W m-2]
    vector<float> input_sw_down;
    // Air temperature [Degree celcius]
    vector<float> input_air_temperature;
    // Soil hydraulic conductivity [m s-1]
    vector<vector<double> > input_k_soil;
    // Soil hydraulic potential [m]
    vector<vector<double> > input_psi_soil;


    std::unique_ptr<Soil_water_module> soil_water_module;

    // Driver values for this timestep
    // Vapour pressure deficit [Pa]
    double ivpd;
    // Shortwave radiation of this timestep [W m-2]
    double isw_down;
    // Atmopheric pressure [Pa]
    double ipressure;
    // Ambient CO2 concentration [ppm]
    double ica;
    // Air temperature [Degree Celcius]
    double itemp_air;

    // List of soil water potentials per soil layer [m] (hydraulic head)
    vector<double> ipsi_soil;
    // List of conductivities per soil layer [m s-1]
    vector<double> ik_soil;

    std::unique_ptr<Water_Potential_Solver> water_potential_solver;

    // Technical parameters
    /// Start time point of the simulations [DateTime]
    DateTime time_start;
    /// End time point of the simulations [DateTime]
    DateTime time_end;

    // Elapsed time in seconds since time_start [s]
    long ts;
    // Mumber of simulated time steps
    int nsteps;
    // Simulation timelength [s]
    double delta_Ts;

    // Functions
    // Get the correct time index from the input forcing
    int time_index(double elapsed_seconds);

    // Output dataset
    Output output;

    void add_output();
};



