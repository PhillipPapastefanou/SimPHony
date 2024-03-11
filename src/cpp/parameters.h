#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <numbers>

class Parameters
{
public:
    Parameters();
    ~Parameters();

    const double SEC_IN_DAY = 86400.0;
    const double CM_IN_M = 100.0;
    const double KG_H2O_To_Mol = 1000.0 / 18.0;
    const double PaToMPa = std::pow(10, -6);
    const double MPaToPa = std::pow(10, +6);
    //const double PI = std::numbers::pi;
    //Todo Use C constants again. Does not work on the MPI cluster atm
    const double PI = 3.14159265359;
    // Density of water [kg m-3]
    double rho_water = 998.0;
    // Gravitational constant [kg m-1 m-2]
    double grav = 9.81;

    // Root zone depth [m]
    double root_zone_depth = 0.3;
    // Root area index [1]
    // Xu et al: 24
    // Katul et al: 5.5 - 14.2
    double root_area_index = 24.0;

    // Plant height [m]
    // From Arend et al 2021 SI
    double canopy_height = 20.0;


    // Viscosity of the leaf to sap flow [1] ??? To be double checked
    // Todo: Check if kinematic or dynamic viscosity
     double eta_LS = 1.0; // 1.0 = Water
    // Stem hydraulic capacity [mol m-3 MPa-1]
    // From Meinzer et al. 2011 Figure 13.2
    // Range: 10 - 500 kg H2O m-3 MPa-1
    // Convert to [mol H2O m-3 MPa-1]
    double stem_hydraulic_capacitance_max = 20.0 * KG_H2O_To_Mol;

    // Xylem saturated Hydraulic conductivity [mol m-1 s-1 MPa-1]
    // Meinzer et al 2010: Could be up 1-10 kg H2O m-1 s-1 MPa-1 (Maximum specific conductivity)
    // Meinzer et al 2010: 0.05- 10 kg H2O m-1 s-1 MPa-1 (Xylem specific conductivity)
    double k_xylem_sat = 150/1800.0;
    // Huber value [m2 m-2] equals 1/klatosa (leaf area to sapwood area)
    // From sperry et al
    double huber_value = 1.0/3600.0;

    // Xylem water potential at loss of 50% conductivity [MPa]
    double psi50_xylem = -3.5;
    // Slope parameter of xylem water potential curve (has to be postive)[-]
    double d_50_s = 5.0;

    // Leaf hydraulic conductance [mol m-2 MPa -1]
    // range 0.2 - 1.2 from Blackmann and Brodribb 2011
    double leaf_hydraulic_capacitance = 1.0;
    // Leaf area index [m2 m-2]
    // Data from the swiss site (half hemispherical)
    double leaf_area_index = 4.8;
    // Leaf water potential at which plants close stomates to 50 % [MPa]
    // Something like this can be made up here...
    double psi_leaf_50_close = -2.1;
    // Slope parameter of stomatal closure
    //  No reference here yet, be careful with the sign
    double d_50_close = 10.0;

    // Medlyn 2011 model g0 parameter [mol m-2 s-1]
    // (also minmal stomatal condutances)
    // From Medlyn 0 - 0.05
    // From the site 0.05
    double g0 = 0.005;
    // Medlyn 2011 model g1 parameter [1]
    // From Medlynn 1.6 - 12
    double g1 = 1.5;

    // Number of soil layers
    int nsoil = 11;
    // Depth of each soil layer [m]
    double layer_depth  = 0.1;
    // Min depth of soil layers (should be zero) [m]
    double min_soil_layer_depth = 0.0;
    // Max depth of soil layers [m]
    double max_soil_layer_depth = 1.0;
    // Jackson rooting parameter [-]
    double jackson_root_beta  = 0.96;

    // Saturated soil hydraulic conductance [m s-1]
    // Van Looy et al 4.8 - 62 [cm d-1]
    double _k_soil_sat_cm_d = 15.0;
    double k_soil_sat = _k_soil_sat_cm_d / CM_IN_M / SEC_IN_DAY;

    // Saturated volumetric water content [m3 m-3]
    double theta_s = 0.8;

    // Empirical water content multiplier [-]
    // Should be 1.0 for runs where the given wcont is known precisely
    double theta_emp_multiplier = 1.0;

    // Van Genuchten parametersmm
    double theta_r = 0.105;
    double alpha_genucht = 1.0;
    double n_genucht = 6.0;
    double neta_genucht = 0.5;

    // Campbell 1974 / Clapp and STuff 1978 shape parameter [1]
    double camp_b = 8.4;
    // Reference Soil water potential [MPa]
    // from Clapp 1978: -15.3 cm
    double camp_psi_soil_ref = -1.50042 * std::pow(10, -6);

    double solver_precision = 1E-10;


    /// Input parameters
    /// Length of one timestep in [s]
    double input_steplen = 1800.0;

private:
};
