#pragma once
#include "../framework/parameters.h"
#include "../io/input_swiss.h"
#include <vector>
#include "soil_water_model.h"
#include "../auxil/solvers.h"
#include "../io/output.h"
#include "model.h"
#include "../framework/date_time.h"
#include "stem_flow_model.h"
#include "../framework/water_potential_solver.h"

class Model{

public:
    Model(Parameters& parameters, Input& input);
    void Set_derived_parameters();
    void Set_initial_conditions(double psi_leaf_zero, double psi_soil_zero);
    void Run(DateTime begin, DateTime end);
    const Output& Get_output();
private:
    // Input references
    const Parameters& params;
    Input& input_module;

    vector<float> input_vpd;
    vector<float> input_anet;

    std::unique_ptr<Soil_water_module> soil_water_module;
    vector<vector<double> > input_k_soil;
    vector<vector<double> > input_psi_soil;

    // Driver values for this timestep
    // Vapour pressure deficit [Pa]
    double ivpd;
    // Net photosythesis rate [mol CO2 m-2 s-1]
    double ianet;
    // Atmopheric pressure [Pa]
    double ipressure;
    // Ambient CO2 concentration [ppm]
    double ica;
    // List of soil water potentials per soil layer [MPa]
    vector<double> ipsi_soil;
    // List of conductivities per soil layer [mol H2O m-1 MPA-1 s-1]
    vector<double> ik_soil;

    std::unique_ptr<Water_Potential_Solver> water_potential_solver;

    // Technical parameters
    /// Start time point of the simulations [DateTime]
    DateTime time_start;
    /// End time point of the simulations [DateTime]
    DateTime time_end;

    // Elapsed time in seconds since time_start [s]
    long ts;
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



