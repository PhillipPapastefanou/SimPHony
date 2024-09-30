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

    void Update_forcing(std::vector<double> psi_soil_sl,
                        std::vector<double> k_soil_sl,
                        double anet,
                        double vpd,
                        double ca,
                        double pressure);


    virtual void Init_solver() = 0;
    virtual void Update_water_potentials() = 0;

    void Update_output(Output& output);

protected:
    void init_base();

    /// Update the stem water potential segments
    void calc_psi_stems();


    const Parameters& params;



    double dts;
    /// Stomatal conductance [mol H2O m-2 s-1]
    double gs;
    /// Downregulation factor of the g1 parameter of the Medyln2011 photosynthesis
    /// 0 --> No stomatal conductance; 1 --> Full stomatal conductance
    double beta_stom_cond;
    // Adjusted gompertz function parameter [MPa]
    double psi_gomp_50;




    /// Main states
    /// Leaf water potential at average canopy height [MPa]
    double psi_leaf;
    /// Root water potential at the bottom of the stem [MPa]
    double psi_stem_ground;


    double psi_leaf_prev_ts;
    double psi_stem_ground_prev_ts;

    /// Stem water potential segments between stem ground and canopy [MPa]
    std::vector<double> psi_stem_segments;


    /// Transpirational water flow [mol H2O m-2 s-1]
    double T;
    /// Stem water flow [mol H2O m-2 dts-1]
    double J;
    /// Total Soil water uptake flow [mol H2O m-2 s-1]
    double G;
    /// Individial soil water uptake flow [mol H2O m-2 s-1]
    std::vector<double> Gi;
    /// Bark water loss [mol H2O m-2 s-1]
    double T_G;


    /// Model input
    // List of soil water potentials per soil layer [m]
    std::vector<double> psi_soil_sl;
    // List of conductivities per soil layer [m s-1]
    std::vector<double> k_soil_sl;
    double anet;
    double vpd;
    double ca;
    double pressure;

    /// Stem water flow models
    std::unique_ptr<Stem_flow_module> stem_flow_module;

    /// Derived states
    double min_leaf_water_potential;

    // Convert to maximum change in MPa per timestep [MPa]
    double max_psi_leaf_change_per_ts;

};


