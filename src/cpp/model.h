#include "parameters.h"
#include "input_swiss.h"
#include <vector>
#include "soil_water_model.h"
#include "solvers.h"
#include "output.h"
#include "date_time.h"

class Bisection_psi_leaf;
class Bisection_psi_stem;

class Leaf_Stem_Implicit_Model{

public:
    Leaf_Stem_Implicit_Model(const Parameters& parameters, const Input& input);

    void Set_derived_parameters();

    void Set_initial_conditions(double psi_leaf_zero, double psi_soil_zero);

    void Run(double steplength, DateTime begin, DateTime end);

    double psi_stem_root(double psi_stem_target);
    double psi_leaf_root(double psi_leaf_target);

    const Output& Get_output();

    /// Length of one timestep [s]
    double dts;

private:
    // Input
    const Parameters& params;
    const Input& input_module;

    vector<float> input_vpd;
    vector<float> input_anet;

    std::unique_ptr<Soil_water_module> soil_water_module;
    vector<vector<double> > input_k_soil;
    vector<vector<double> > input_psi_soil;

    // Driver values for this timestep

    // Vapour pressure deficit [Pa]
    double ts_vpd;
    // Net photosythesis rate [mol CO2 m-2 s-1]
    double ts_anet;
    // Atmopheric pressure [Pa]
    double ts_pressure;
    // Ambient CO2 concentration [ppm]
    double ts_ca;


    // Derived parameters
    double psi_gomp_50;


    // Output dataset
    Output output;

    /// Main states
    /// Leaf water potential at average canopy height [MPa]
    double psi_leaf;

    /// Stem water potential at the mean height of the stem [MPa]
    double psi_stem;

    // Derivative of the leaf water potential. Internal function.
    double d_psi_leaf(double psi_leaf, double psi_stem);

    // Derivative of the stem water potential. Internal function.
    double d_psi_stem(double psi_leaf, double psi_stem);


    // List of soil water potentials per soil layer [MPa]
    vector<double> ts_psi_soil;
    // List of conductivities per soil layer [m s-1]
    vector<double> ts_k_soil;

    // Derived Parameters
    vector<double> root_fraction_player;
    vector<double> soil_layer_depth_acc;


    /// Solvers
    std::unique_ptr<Bisection_psi_leaf> solver_psi_leaf;
    std::unique_ptr<Bisection_psi_stem> solver_psi_stem;


    /// Derived or helper states
    /// Forcing pressure between leaf and soil water potential [MPA]
    /// Driving force for the water to be taken up the plant
    double DeltaP_LS;

    /// Stomatal conductance [mol H2O m-2 s-1]
    double gs;

    /// Downregulation factor of the g1 parameter of the Medyln2011 photosynthesis
    /// 0 --> No stomatal conductance; 1 --> Full stomatal conductance
    double beta_stom_cond;

    /// Transpirational water flow [mol m-2 dts-1]
    double T;
    /// Stem water flow [mol m-2 dts-1]
    double J;
    /// Total Soil water uptake flow [mol m-2 dts-1]
    double G;
    /// Individial soil water uptake flow [mol m-2 dts-1]
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

    // Helper function for the stem water flow. Should only be called only once per either stem water pot estimation
    // or leaf water pot estimation
    double update_stem_water_flow_J(double psi_leaf, double psi_stem);


    void add_output();
};


class Bisection_psi_leaf : public Bisection_solver_interface {

public:
    Bisection_psi_leaf(Leaf_Stem_Implicit_Model& model , double precision, int max_steps): Bisection_solver_interface(model, precision, max_steps){}
protected:
    double f(double x) override;
};

class Bisection_psi_stem : public Bisection_solver_interface {

public:
    Bisection_psi_stem(Leaf_Stem_Implicit_Model& model , double precision, int max_steps): Bisection_solver_interface(model, precision, max_steps){}
protected:
    double f(double x) override;
};
