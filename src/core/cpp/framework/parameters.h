#pragma once
#include <string>
#include <vector>
#include <cmath>
#include "../modules/soil_layer.h"
//#include <numbers>

enum class Conductivity_fraction_module_type{
    Weibull,
    Logit,
};

enum class Stem_flow_module_type{
    Linear,
    KirchhoffWeibull,
    KirchhoffPiecewiseErf
};

enum class Soil_water_module_type{
    Campbell,
    Saxton06,
    VanGenuchten
};



struct Constants{

    public:
    const double SEC_IN_DAY = 86400.0;
    const double SEC_IN_HOUR = 3600.0;
    const double CM_IN_M = 100.0;
    const double KG_H2O_To_Mol = 1000.0 / 18.0;
    const double G_H2O_To_Mol = 1.0 / 18.0;
    const double KG_to_G = 1000.0;
    const double KPaToPa = 1000.0;
    const double PaToMPa = std::pow(10, -6);
    const double MPaToPa = std::pow(10, +6);
    const double H2O_TO_CO2_DIFFUSIVITY = 1.6;
    const double PPM_TO_PA = 101326.0 / 1E6;
    const double PA_TO_PPM = 1000.0 / 101.325;
    const double UMOL_TO_MOL = std::pow(10, -6);
    //const double PI = std::numbers::pi;
    //Todo Use C constants again. Does not work on the MPI cluster atm
    const double PI = 3.14159265359;
    // Density of water [kg m-3]
    const double RHO_WATER = 998.0;
    // Gravitational constant [kg m-1 s-2]
    const double GRAVITY = 9.81;

    const double HydraulicHeadInMtoMPa =  GRAVITY / 1000.0;
    const double MPaToHydraulicHeadM =  1.0 / HydraulicHeadInMtoMPa;

    // Maximum leaf water potential [MPa]
    double MAX_LEAF_WATER_POTENTIAL = -1E-12;
    // Maximum stem water potential [MPa]
    double MAX_STEM_WATER_POTENTIAL = -1E-16;
    // Number of milliseconds to print console output
    const double TMUTE_MILLISEC = 10000.0;

};


class Parameters
{
public:
    Parameters();
    ~Parameters();
    void Set_derived();

    // Universal parameter setting ID
    int id = -1;

    // Parameters that assumed to be constants and will not change
    Constants constants;

    // Root area index [1]
    // Katul et al: 5.5 - 14.2
    double root_area_index = 10.0;

    // Plant height [m]
    double canopy_height = 20.0;

    // Stem flow module type
    //    Linear,
    //    KirchhoffWeibull,
    //    KirchhoffPiecewiseErf
    Stem_flow_module_type stem_flow_type = Stem_flow_module_type::Linear;

    // Viscosity of the leaf to sap flow [1]
    // Todo: Check if kinematic or dynamic viscosity
     double eta_LS = 1.0; // 1.0 = Water

    // Stem hydraulic capacity [mol m-3 MPa-1]
    // From Meinzer et al. 2011 Figure 13.2
    // Range: 10 - 500 kg H2O m-3 MPa-1
    // Convert to [mol H2O m-3 MPa-1]
    double stem_hydraulic_capacitance_max = 100 * constants.KG_H2O_To_Mol;

    // Xylem saturated hydraulic conductivity [mol m-1 s-1 MPa-1]
    // Typical range between 0.5 and 10 kg m-1 s-1 MPa-1 (Xu et al. 2016)
    double k_xylem_sat = 1.0 * constants.KG_H2O_To_Mol;
    // Huber value [m2 m-2] equals 1/klatosa (leaf area to sapwood area)
    // Typical ranges cover 1/20000 to 1/2000
    double huber_value = 1.0/3600.0;

    // Heartwood area to sapwoo area ratio [1]
    double ratio_heart_sap_area = 3.0;
    // Conductivity or diffusivity between sap and heartwood [mol m-1 s-1 MPa-1]
    // Todo ensure if its bulk water transport or a diffusive process or both?
    // Futhermore also take care of all the units and areas this water is flowing through
    double k_heart_sap = 10.0;

    // Xylem water potential at loss of 50% conductivity [MPa]
    double psi50_xylem = -3.5;
    // Xylem water potential at loss of 88% conductivity [MPa]
    double psi88_xylem = -6.0;
    // functional shape of xylem loss of conducitivity function
    // Weibull is the more accept throughout the literature and is asymmetrical
    // Logit is symmetrical and can be solved analyyically when evaluated in the Kirchhoff transition
    Conductivity_fraction_module_type  conductivity_fraction_type = Conductivity_fraction_module_type::Weibull;

    // Leaf hydraulic conductance [mol H2O m-2 MPa-1]
    // range 0.2 - 1.2 from Blackmann and Brodribb 2011
    // about 0.05 (Xu et al. 2016)
    double leaf_hydraulic_capacitance = 1.0;

    // Leaf area index [m2 m-2]
    // Data from the swiss site (half hemispherical)
    double leaf_area_index = 4.8;

    // Leaf water potential at which plants close stomates to 50 % [MPa]
    // Note: This is only evaluated once at typically 5 am. 
    double psi_leaf_50_close = -1.5;

    // Slope parameter of stomatal closure
    // No reference here yet, be careful with the sign
    // Has to be positive
    double d_50_close = 2.0;

    // Medlyn 2011 model g0 parameter [mol m-2 s-1]
    // (also minmal stomatal condutances)
    // From Medlyn 2011: 0 - 0.05
    // From the site 0.05
    double g0 = 0.005;
    // Medlyn 2011 model g1 parameter [kPa-1]
    // From Medlynn 1.6 - 12
    double g1 = 1.5;
    // Maximum carboxylation rate at 25 Degree celcius
    double vmax25 = 43;
    // Maximum electron transport rate at 25 Degree celcius
    double jmax25 = 71;


    // Stem residucal condutance [mol m-2 s-1]
    // We have to check how the diameter and the stem surface area will influence that
    double g_stem_res = 0.0;

    // Soil water module type [enum]
    Soil_water_module_type soil_water_type = Soil_water_module_type::VanGenuchten;


    // Index of current soil profile index if multiple water contents per sites are available
    // Only relevant for
    int soil_profile_index = 0;

    // Jackson rooting parameter [-]
    double jackson_root_beta  = 0.96;

    // Number of standard deviation (sd) that the water content is moved away from the mean sensors
    // Choose 0.0 to have the mean water content, 1.0 equals to 1SD higher water content
    double wcont_sigma_deviation = 0.0;

    // Tree density [Trees m-2]
    // Used for output scaling onlny
    double tree_density = 1.0;

    // Auxiliary parameters

    // Number of stem segments between psi_L und psi_S
    double n_stem_segments = 10;

    // Whether xylem damage is permanent or not to each segment
    bool sustain_xylem_damage = false;
    // Parameter describing at which levels xylem damage is permanent
    double permanent_xylem_fraction_threshold = 0.88;

    // Analysis_Swiss parameter [-]
    double sigma_log_likelyhood = 1.0;


    // Maximum leaf water potential change per hour [MPa]
    double max_psi_leaf_change_per_hour = 5.0;
    // Multiplier to estimate the minimum leaf water potential
    // psi_leaf_min = muliplier x psi_88
    double minimum_psi_leaf_multiplier = 1.5;

    // Todo connect to solvers
    double solver_precision = 1E-10;


    bool verbose = false;

    //Simulation timestep length [s]
    double dts = 1800.0;

    /// Input parameters
    /// Length of one timestep in [s]
    double dts_input = 1800.0;

    // Soil layers containing soil properties
    std::vector<Soil_layer> soil_layers;

private:

};
