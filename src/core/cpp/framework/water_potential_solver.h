//
// Created by Phillip on 25.04.24.
//

#pragma once
#include "../framework/parameters.h"
#include "../modules/stem_flow_model.h"
#include "../io/output.h"
#include "../auxil/solvers.h"



class Water_Potential_Solver {

public:
    Water_Potential_Solver(const Parameters& params);
    virtual ~Water_Potential_Solver();

    void Init_water_potentials(double psi_leaf_init, double psi_stem_base_init);

    void Update_input(std::vector<double> psi_soil_sl,
                      std::vector<double> k_soil_sl,
                      double gs,
                      double vpd,
                      double pressure);


    virtual void Init_solver() = 0;
    virtual void Update_water_potentials(DateTime dt) = 0;
    virtual double Get_beta() = 0;

    void Update_output(Output& output);

    /// Root water uptake per soil layer [mol H2O m-2 s-1 per unit LEAF area, matching
    /// T/J/Output -- see its computation in solver_indiv_eulerimp.cpp's d_psi_stem_ground],
    /// as computed for the timestep of the most recent Update_water_potentials() call. Used
    /// to feed a sink term back into a prognostic soil hydrology model
    /// (Soil_hydrology_richards) -- Model::Run() converts it to a per-ground-area value at
    /// that point of use, since a soil water balance needs to deplete a real 1 m^2 column.
    /// Otherwise only used for output/diagnostics.
    const std::vector<double>& Get_root_uptake_indiv() const { return Gi; }

protected:
    void init_base();

    /// Update_photosythesis the stem water potential segments
    void calc_psi_stems();


    const Parameters& params;

    // Timestep length [s]
    double dts;
    /// Stomatal conductance [mol H2O m-2 s-1]
    double gs;
    /// Downregulation factor of the g1 parameter of the Medyln2011 Assimilation
    /// 0 --> No stomatal conductance; 1 --> Full stomatal conductance
    double beta_stom_cond;
    // Adjusted gompertz function parameter [MPa]
    double psi_gomp_50;

    /// Main states
    /// Leaf water potential at average canopy height [MPa]
    double psi_leaf;
    /// Sapwood stem water potential at the bottom of the stem [MPa]
    double psi_sap_ground;
    /// Heartwood stem water potential at the bottom of the stem [MPa]
    double psi_heart_ground;

    
    
    /// Leaf water potential of previous timeste [MPa]
    double psi_leaf_prev_ts;
    /// Stem water potential at the bottom of the stem previous timestep [MPa]
    double psi_stem_ground_prev_ts;

    /// Stem water potential segments between stem ground and canopy [MPa]
    std::vector<double> psi_stem_segments;


    /// Transpirational water flow [mol H2O m-2 s-1]
    double T;
    /// Sap water flow [mol H2O m-2 s-1]
    double J;
    /// Lateral water flow between sapwood and heartwood [mol H2O m-2 s-1]
    double O;
    /// Total Soil water uptake flow [mol H2O m-2 s-1]
    double G;
    /// Soil water uptake per layer [mol H2O m-2 s-1]
    std::vector<double> Gi;
    /// Residual water loss [mol H2O m-2 s-1]
    double T_res;


    /// Model input
    // List of soil water potentials per soil layer [m]
    std::vector<double> psi_soil_sl;
    // List of conductivities per soil layer [m s-1]
    std::vector<double> k_soil_sl;

    // Vapour pressure deficit [Pa]
    double vpd;
    // Atmopheric pressure [Pa]
    double pressure;

    /// Stem water flow models
    std::unique_ptr<Stem_flow_module> stem_flow_module;

    /// Derived states
    double min_leaf_water_potential;

    // Convert to maximum change in MPa per timestep [MPa]
    double max_psi_leaf_change_per_ts;

};


