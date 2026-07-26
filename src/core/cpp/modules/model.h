#pragma once
#include "../framework/parameters.h"
#include "../io/input.h"
#include <vector>
#include "soil_water/soil_water_model.h"
#include "soil_water/soil_hydrology_richards.h"
#include "../auxil/solvers.h"
#include "../io/output.h"
#include "model.h"
#include "../framework/date_time.h"
#include "stem_flow_model.h"
#include "../framework/water_potential_solver.h"

class Model{

public:
    Model(Parameters& parameters, Input& input, Config& config);
    // Precalculates soil water potential/conductivity over the *entire*
    // loaded forcing series. Kept for existing callers (Simulation_Multi,
    // several tests) that don't know their simulation window up front.
    void Set_derived_parameters();
    // Same, but restricts the (expensive) soil-water precalculation to
    // [begin, end] instead of the whole forcing series, since Run(begin, end)
    // never reads outside that window anyway. Prefer this overload whenever
    // the window is known ahead of time (e.g. Simulation_Single::Run).
    void Set_derived_parameters(DateTime begin, DateTime end);
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
    // Soil volumetric water content per layer [m3 m-3]
    vector<vector<float> > input_theta;


    std::unique_ptr<Soil_water_module> soil_water_module;
    // Only constructed when params.use_prognostic_soil_hydrology is true; owns the
    // per-timestep soil moisture state in that case instead of input_psi_soil/input_k_soil/
    // input_theta (which stay only partially populated -- see Set_derived_parameters_impl).
    std::unique_ptr<Soil_hydrology_richards> soil_hydrology_richards;

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
    // List of volumetric water contents per soil layer [m3 m-3]
    vector<float> itheta;

    // Prognostic soil hydrology diagnostics for this timestep [kg m-2 s-1]; only meaningful
    // (and only logged by add_output()) when params.use_prognostic_soil_hydrology is true.
    double iprecip = 0.0;
    double iinfiltration = 0.0;
    double irunoff = 0.0;
    double idrainage = 0.0;

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

    // Shared implementation for both Set_derived_parameters() overloads.
    // soil_start_idx/soil_end_idx are forwarded to
    // Soil_water_module::CalculatePsiAndKs() -- see its declaration for
    // what they mean; (0, -1) means "the whole forcing series".
    void Set_derived_parameters_impl(int soil_start_idx, int soil_end_idx);
};



