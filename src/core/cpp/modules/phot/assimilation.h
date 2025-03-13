//
// Created by Phillip on 10/03/2025.
//

#ifndef SIMPHONY_ASSIMILATION_H
#define SIMPHONY_ASSIMILATION_H
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <optional>
#include "../../framework/parameters.h"

class Assimilation {


public:
    Assimilation(const Parameters& parameters);

    void Update_photosythesis(double tair, double sw_rad, double ca, double gs_over_anet);

    double Update_gs_over_anet(double beta, double an, double ci_ppm, double vpd);
    /**
     * @brief Calculates net photosynthesis (Anet) and stomatal conductance (gs).
     *
     * This function computes the net rate of carbon dioxide assimilation by a plant
     * (Anet) and the stomatal conductance (gs), which is a measure of the rate of
     * gas exchange through the stomata (pores) on the leaves.
     *
     * @param vpd  Vapor Pressure Deficit (kPa). This represents the difference
     * between the saturation vapor pressure and the actual vapor
     * pressure of the atmosphere.
     * @param tair  Air temperature (K). The temperature of the air surrounding
     * the plant.
     * @param beta  Scaling factor. This factor accounts for various environmental
     * stresses or adjustments, such as soil moisture limitations.
     * @param sw_rad Shortwave radiation (W/m^2). The amount of solar radiation
     * incident on the plant.
     * @param ca_ppm Atmospheric CO2 concentration (ppm). The concentration of
     * carbon dioxide in the air surronding the leaf.
     */
    void Solve_Anet_gs(double vpd, double tair, double beta,  double sw_rad, double ca_ppm);

    double Get_An(){
        return An;
    }

    double Get_Aj(){
        return Aj;
    }

    double Get_Gs(){
        return gs;
    }


private:
    // Net photosynthesis assuming one big leaf exposed to direct sunlight
    // [μmol m-2 s-1]
    double An;
    // [μmol m-2 s-1]
    double Aj;
    // [μmol m-2 s-1]
    double Ac;
    // Stomatal conductance [mol m-2 s-1]
    double gs;

    //double leaf_temperature(double transpiration);

    double arrhen(double v25, double Ea, double Tref, double Tleaf,
                  std::optional<double> deltaS = std::nullopt,
                  std::optional<double> Hd = std::nullopt);
    double foliar_resp(double Tleaf);
    double adjust_low_T(double var, double Tleaf, double lower_bound = 2.0, double upper_bound = 10.0);
    double quad(double a, double b, double c, bool large_root = true);
    double quad_solve_Ci(double Cs, double gs_over_A, double Rleaf, double gamstar,
                         double v1, double v2);
    double calc_gb(double Tair);


    const Parameters& parameters;
    //# SW (W m-2) to PAR (umol m-2 s-1)
    const double SW_2_PAR = 4.57 * 0.5;
    const double C_2_K = 273.15;
    const double zero = 1e-10;
    const double R = 8.314462618;
    // heat capacity of dry air, cst P (J mol-1 K-1)
    const double Cp = 29.29;
    // Gas concentrations
    double CO2 = 37.0;
    double O2 = 20.73;
    const double ref_kPa = 101.325; //  # to ref kPa
    const double FROM_kPa = 1. / ref_kPa ;// # from kPa to anything
    const double MILI = 1.e3;
    const double FROM_MILI = 1.e-3;
    const double PA_TO_UMOL_PER_MOL = 1000.0  / 101.325;
    const double Mair = 28.9644;  // # molar mass of dry air (g mol-1)
    const double DH = 21.5e-6 ; // # molecular diffusivity to heat (m2 s-1)
    // Windspeed component
    const double u = 2.0; //  # m s-1

    const double GbhvGb = 0.93; //  # ratio of Gbheat:Gbwater
    const double GbvGbh = 1.0 / GbhvGb; //  # Gbwater:Gbheat
    const double GwvGc = 1.57; //# Gwater:GCO2

    const double GcvGw = 1. / GwvGc;  //# GCO2:Gwater
    const double GbvGbc = std::pow(GwvGc , 2. / 3.);  //# Gbwater:GbCO2
    const double GbcvGb = 1. / GbvGbc; //  # GbCO2:Gbwater


    double Patm = 101.325;  // kPa

    // Canopy structure & interception
    double LAI = 2.0;
    double max_leaf_width = 0.05;
    double can_sat = 10.0;
    double kcanint = 0.7;

    // Photosynthetic parameters
    double Vmax25 = 100.0;
    double Vmaxmin = 10.0;
    double Vmaxmax = 180.0;
    double gamstar25 = 4.22;
    double Tref = 25.0;
    double JV = 1.67;
    double JVmin = 1.0;
    double JVmax = 3.0;
    double Rlref = Vmax25 * 0.015;
    double TRlref = 25.0;
    double Kc25 = 39.96;
    double Ko25 = 27.48;
    double alpha = 0.3;

    // Farquhar model
    double c1 = 0.7;
    double c2 = 0.99;

    // Collatz model
    double c3 = 0.83;
    double c4 = 0.93;

    // Energies of activation
    double Ev = 60000.0;
    double Ej = 30000.0;
    double Egamstar = 37830.0;
    double Ec = 79430.0;
    double Eo = 36380.0;

    // Inhibition at higher temperatures (Kattge & Knorr)
    double deltaSv = 650.0;
    double deltaSj = 650.0;
    double Hdv = 200000.0;
    double Hdj = 200000.0;

    // Relating to light / rad (C & N is Campbell & Norman)
    double eps_l = 0.97;
    double eps_s = 0.945;
    double albedo_l = 0.062;
    double albedo_ws = 0.1;
    double albedo_ds = 0.25;
    double tau_l = 0.05;
    double chi_l = 9.99999978e-3;
    double kn = 0.001;

    // Leaf nitrogen (Evans; Medlyn)
    double aj = 15870.0;
    double bj = 2775.0;
    double kcat = 24.0;
    double ks = 1.25e-4;
    double ccN = 0.076 / 25.0;

    // Stomatal conductance
    double sfw = 0.2;



};


#endif //SIMPHONY_ASSIMILATION_H
