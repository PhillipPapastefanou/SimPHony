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

class Bisection_psi_leaf;
class Bisection_psi_stem_ground;

class Leaf_Stem_Ground_Implicit_Model{

public:
    Leaf_Stem_Ground_Implicit_Model(const Parameters& parameters, const Input& input);

    void Set_derived_parameters();

    void Set_initial_conditions(double psi_leaf_zero, double psi_soil_zero);

    void Run(double steplength, DateTime begin, DateTime end);

    double psi_stem_root(double psi_stem_target);
    double psi_leaf_root(double psi_leaf_target);

    const Output& Get_output();

    /// Length of one timestep [s]
    double dts;

private:
    // Input references
    const Parameters& params;
    const Input& input_module;

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


    // Derived parameters

    // Adjusted gompertz function parameter [MPa]
    double psi_gomp_50;
    // Rooting fraction per laye [-]
    vector<double> root_fraction_player;
    // Soil layer depths
    vector<double> soil_layer_depth_acc;
    // Minimum leaf water potential
    double min_leaf_water_potential;

    /// Stem water flow models
    std::unique_ptr<Stem_flow_module> stem_flow_module;

    /// Main model solvers
    std::unique_ptr<Bisection_psi_leaf> solver_psi_leaf;
    std::unique_ptr<Bisection_psi_stem_ground> solver_psi_stem_ground;

    /// Main states
    /// Leaf water potential at average canopy height [MPa]
    double psi_leaf;
    /// Root water potential at the bottom of the stem [MPa]
    double psi_stem_ground;
    /// Stem water potential segments between stem ground and canopy [MPa]
    std::vector<double> psi_stem_segments;


    // Main functions
    /// Derivative of the leaf water potential. Internal function [MPa s-1]
    double d_psi_leaf(double psi_leaf, double psi_stem);
    /// Derivative of the root water potential. Internal function.
    double d_psi_stem_ground(double psi_leaf, double psi_stem);
    /// Update the stem water potential segments
    void update_psi_stems();

    /// Stomatal conductance [mol H2O m-2 s-1]
    double gs;

    /// Downregulation factor of the g1 parameter of the Medyln2011 photosynthesis
    /// 0 --> No stomatal conductance; 1 --> Full stomatal conductance
    double beta_stom_cond;

    /// Transpirational water flow [mol H2O m-2 s-1]
    double T;
    /// Stem water flow [mol H2O m-2 dts-1]
    double J;
    /// Total Soil water uptake flow [mol H2O m-2 s-1]
    double G;
    /// Individial soil water uptake flow [mol H2O m-2 s-1]
    vector<double> Gi;

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


class Bisection_psi_leaf : public Bisection_two_layer_solver_interface {

public:
    Bisection_psi_leaf(Leaf_Stem_Ground_Implicit_Model& model , double precision, int max_steps, double global_min, double global_max):
    Bisection_two_layer_solver_interface(model, precision, max_steps, global_min, global_max){}
protected:
    double f(double x) override;
};

class Bisection_psi_stem_ground : public Bisection_two_layer_solver_interface {

public:
    Bisection_psi_stem_ground(Leaf_Stem_Ground_Implicit_Model& model , double precision, int max_steps, double global_min, double global_max ):
    Bisection_two_layer_solver_interface(model, precision, max_steps, global_min, global_max){}
protected:
    double f(double x) override;
};
