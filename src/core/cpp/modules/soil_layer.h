//
// Created by Phillip on 02.05.24.
//
#pragma  once
struct Soil_layer {

public:
    // Soil layer depth [m]
    double depth;

    // Organic matter fraction [0-1]
    double organic_matter_fraction;
    // Sand fraction [0-1]
    double sand_fraction;
    // Clay fraction [0-1]
    double clay_fraction;

    // Saturated soil hydraulic conductivity [m s-1]
    double k_soil_sat;
    // Reference Soil water potential in hydraulic head [m]
    // from Clapp 1978: -15.3 cm
    double psi_soil_sat = -0.153;

    // Saturated volumetric water content [m3 m-3]
    double theta_s = 0.6;
    // Residual volumetric water content [m3 m-3]
    double theta_r = 0.105;
    // Needed for van genuchten [-]
    double pore_size_ind = 0.5;
    // Campbell 1974 / Clapp and Stuff 1978 shape parameter [1]
    double camp_b = 8.4;

    // Derived parameters
    // Rooting fraction is calculated according yto jacsons distribution
    double root_fraction;
};





