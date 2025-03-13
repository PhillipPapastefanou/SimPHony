//
// Created by Phillip on 10/03/2025.
//

#include "assimilation.h"
#include <iostream>


Assimilation::Assimilation(const Parameters &parameters): parameters(parameters) {

}

void Assimilation::Update_photosythesis(double tair, double sw_rad, double ca, double gs_over_anet) {

    // tair [Degree Celcius]
    // sw_rad [M m-2]
    // ca [Pa]
    // gs_over_anet [dimensionless]

    //double Tleaf = leaf_temperature(transpiration);

    // For now we assume that leaf temperature equals air temperature.
    double Tleaf = tair;

    double Rleaf = foliar_resp(Tleaf);

    double tref_h = Tref + C_2_K;

    double gamstar = arrhen(gamstar25, Egamstar, tref_h, Tleaf);

    double Vmax = arrhen(Vmax25, Ev, tref_h, Tleaf, deltaSv, Hdv);

    double Jmax = arrhen(JV * Vmax25, Ej, tref_h, Tleaf, deltaSj, Hdj);

    Vmax = adjust_low_T(Vmax, Tleaf);
    Jmax = adjust_low_T(Jmax, Tleaf);

    double Kc = arrhen(Kc25, Ec, tref_h, Tleaf);
    double Ko = arrhen(Ko25, Eo, tref_h, Tleaf);

    Ko = std::max(zero, Ko);

    double Km = Kc * (1.0 + O2 / Ko);

    double scale2can = 1.0;
    Rleaf *= scale2can;
    Vmax *= scale2can;
    Jmax *= scale2can;

    double ci = quad_solve_Ci(ca, gs_over_anet, Rleaf, gamstar, Vmax, Km);

    if (ci <= zero || ci > ca || std::isnan(ci)) {
        Ac = 0.0;
    } else {
        Ac = Vmax * (ci - gamstar) / (ci + Km);
    }

    double PPFD = sw_rad * SW_2_PAR;

    double J;
    std::string photo = "Farquahar";

    if (photo == "Farquhar") {
        J = quad(c1, -((1.0 - tau_l - albedo_l) * alpha * PPFD + Jmax), (1.0 - tau_l - albedo_l) * alpha * PPFD * Jmax,
                 false);
    } else {
        J = (1.0 - tau_l - albedo_l) * alpha * PPFD;
    }

    J *= 0.25;


    ci = quad_solve_Ci(ca, gs_over_anet, Rleaf, gamstar, J, 2. * gamstar);

    Aj = J * (ci - gamstar) / (ci + 2.0 * gamstar);
    if (ci - gamstar <= zero || ci > ca || std::isnan(ci)) {
        ci = ca;
        Aj = J * (ci - gamstar)/ (ci + 2.0 * gamstar);
    }



    An = std::min(Aj, Ac) - Rleaf;

    //No diagnostics regarding RuBisCO limitation.
}

double Assimilation::foliar_resp(double Tleaf) {
    double Q10 = 3.22 - 0.046 * Tleaf;
    return Rlref * pow(Q10, (Tleaf - TRlref) / 10.);
}

double Assimilation::arrhen(double v25, double Ea, double Tref, double Tleaf, std::optional<double> deltaS,
                            std::optional<double> Hd) {
    double Tl_K = Tleaf + C_2_K;
    double arrhenius = v25 * exp(Ea * (Tl_K - Tref) / (Tref * R * Tl_K));

    if (!deltaS.has_value() || !Hd.has_value()) {
        return arrhenius;
    } else {
        double arg2 = 1. + exp((*deltaS * Tref - *Hd) / (R * Tref));
        double arg3 = 1. + exp((*deltaS * Tl_K - *Hd) / (R * Tl_K));
        return arrhenius * arg2 / arg3;
    }
}

double Assimilation::adjust_low_T(double var, double Tleaf, double lower_bound, double upper_bound) {
    if (Tleaf < lower_bound) {
        var = 0.0;
    } else if (Tleaf < upper_bound) {
        var *= (Tleaf - lower_bound) / (upper_bound - lower_bound);
    }
    return var;
}

double Assimilation::quad(double a, double b, double c, bool large_root) {
    double discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0) {
        // Handle the case of complex roots if necessary
        // For now, we'll just return 0.0
        return 0.0;
    }
    double root1 = 0.5 * (-b + std::sqrt(discriminant)) / a;
    double root2 = 0.5 * (-b - std::sqrt(discriminant)) / a;
    return large_root ? root1 : root2;
}

double Assimilation::quad_solve_Ci(double Cs, double gs_over_A, double Rleaf, double gamstar, double v1, double v2) {
    // Unit conversions, from Pa to μmol mol-1
    double Csi = Cs * MILI * FROM_kPa;
    double gammastar = gamstar * MILI * FROM_kPa;
    double V2 = v2 * MILI * FROM_kPa;

    double g0 = 1e-9;

    double a = g0 + gs_over_A * (v1 - Rleaf);
    double b = ((1.0 - Csi * gs_over_A) * (v1 - Rleaf) + g0 * (V2 - Csi) -
                gs_over_A * (v1 * gammastar + V2 * Rleaf));
    double c = -((1.0 - Csi * gs_over_A) * (v1 * gammastar + V2 * Rleaf) +
                 g0 * V2 * Csi);

    double ref_root = quad(a, b, c) * ref_kPa * FROM_MILI;

    if (ref_root > Cs || ref_root < zero) {
        return quad(a, b, c, false) * ref_kPa * FROM_MILI;
    } else {
        return ref_root;
    }
}

double Assimilation::calc_gb(double Tair) {
    double TairK = Tair + C_2_K;

    double Patm = 101.312;
    double cmolar = Patm * MILI / (R * TairK);

    // Sutherland Eq for dynamic viscosity
    double mu = 1.458e-6 * pow(TairK, 1.5) / (TairK + 110.4);

    // kinematic viscosity
    double nu = mu * R * TairK / (Patm * Mair);
    double prandtl = nu / DH;

    // boundary layer cond to forced convect. (Campbell & Norman, 1998)
    double d = 0.72 * max_leaf_width;
    double reynolds = u * d / nu;
    double gHa = (0.664 * cmolar * DH * pow(reynolds, 0.5) * pow(prandtl, (1. / 3.)) / d);

    // boundary layer conductance to water vapour
    if (gHa > zero) {
        return std::max(zero, gHa * GbvGbh);
    } else {
        return zero;
    }
}

double Assimilation::Update_gs_over_anet(double beta, double an, double ci_ppm, double vpd) {
    // Avoid division by zero
    double an_scale = std::max(0.1, an);
    // Todo: Fix solver to also convert when dividing g0 by an_scale
    return parameters.g0 + (1.0 + beta * parameters.g1 / std::sqrt(vpd)) / ci_ppm;
    //return parameters.g0/an_scale + (1.0 + beta * parameters.g1 / std::sqrt(vpd)) / ci_ppm;
}

void Assimilation::Solve_Anet_gs(double vpd, double tair, double beta, double sw_rad, double ca_ppm) {

    double Patm = 101.312;

    // Avoid divid by zero
    vpd = std::max(vpd, 0.05);

    gs = 0.0;
    An = 10;
    double gb = 0.0;
    double boundary_co2 = 0.0;
    double ci_pa_prev = 1E9;

    // Initiliase with ambient conditions [ppm]
    double ci_ppm = ca_ppm;
    double ci_pa = ci_ppm * parameters.constants.PPM_TO_PA;
    // Calculate also in [Pa]
    const double ca_pa = ca_ppm * parameters.constants.PPM_TO_PA;

    double gs_over_anet = Update_gs_over_anet(beta, An, ci_ppm, vpd);


    int NMAX = 1000;
    const double EPS_CI = 1E-3;

    bool converged = false;
    for (int i = 0; i < NMAX; ++i) {

        ci_pa_prev = ci_pa;

        Update_photosythesis(tair, sw_rad, ca_pa, gs_over_anet);

        ci_ppm = ci_pa * parameters.constants.PA_TO_PPM;

        gs_over_anet = Update_gs_over_anet(beta, An, ci_ppm, vpd);

        gs = std::max(zero, GwvGc * gs_over_anet * An);

        gb = calc_gb(tair);

        boundary_co2 =  (Patm * FROM_MILI * An)/(gb * GbcvGb);

        ci_pa = std::max(zero, std::min(ca_pa, ca_pa - boundary_co2));

        if (std::abs(ci_pa - ci_pa_prev) < EPS_CI){
            converged = true;
            break;
        }

    }


    if (!converged){
        std::cout << "Assimilation routine did not converge" << std::endl;
        exit(99);
    }


}

//double Assimilation::leaf_temperature(double transpiration, double Tair, double TLeaf_prev, double Rnet) {
//    // unit conversion
//    double TairK = Tair + C_2_K; // degK
//    // get conductances
//    auto [gH, gb, dummy] = conductances(TLeaf_prev); // mol m-2 s-1
//
//    // latent heat of water vapor
//    double Lambda = LH_water_vapour(); // J mol-1
//
//    // slope of saturation vapour pressure of water
//    double slp = slope_vpsat(); // kPa degK-1
//
//    // canopy/leaf sensible heat flux
//    double H = Rnet - Lambda * transpiration; // W m-2

//    if (gradis != 0.0) { // update canopy/leaf sensible heat flux
//        double TleafK = Tleaf + C_2_K; // degK
//        H -= (TleafK - TairK) * cst.Cp * gH * gradis / (gH + gradis);
//    }

//    double TLeaf = 0.0;
//
//    // simplified Tleaf (gb for gw), eq 14.6 of Campbell & Norman, 1998
//    if (std::abs(Tair) < zero) {
//        TLeaf = (Tair + H / (Cp * gH * TairK / zero + Lambda * slp * gb / Patm)); // degC
//    } else {
//        TLeaf = (Tair + H / (Cp * gH * TairK / Tair + Lambda * slp * gb / Patm)); // degC
//    }
//    return TLeaf;
//}
