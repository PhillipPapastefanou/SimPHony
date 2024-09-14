//
// Created by Phillip on 02.05.24.
//
#pragma  once
struct Soil_layer {

public:
    double organic_matter_fraction;
    double sand_fraction;
    double clay_fraction;

    // Saturated soil hydraulic conductivity [m s-1]
    double k_soil_sat;
    // Soil layer depth [m]
    double depth;
    double root_fraction;

    // Saturated volumetric water content [m3 m-3]
    double theta_s = 0.8;

    // Van Genuchten parameters
    double theta_r = 0.105;

    // Reference Soil water potential in hydraulic head [m]
    // from Clapp 1978: -15.3 cm
    double psi_soil_sat = -0.153;

    // Needed for van genuchten
    double pore_size_ind = 0.5;

    // Campbell 1974 / Clapp and Stuff 1978 shape parameter [1]
    double camp_b = 8.4;

};





