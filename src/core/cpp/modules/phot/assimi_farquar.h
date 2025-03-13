//
// Created by Phillip on 13/03/2025.
//


#ifndef SIMPHONY_ASSIMI_FARQUQR_H
#define SIMPHONY_ASSIMI_FARQUQR_H
#include "../../framework/parameters.h"

class Assimi_Farquar {

public:


    Assimi_Farquar(const Parameters& parameters);

    /*
     * @brief Calculates photosynthesis (Farquhar et al. 1980/Collatz et al 1991)
     *
     *
     * @param ppfd  - Active photon flux density (micromol * s-1 * m-2)
     * @param catm CO2 air concentration (micromol * mol-1)
     * @param gs  CO2 stomatal conductance (mol * s-1 * m-2)
     * @param tleaf leaf temperature (ºC)
     * @param jmax298 - maximum electron transport rate per leaf area at 298 ºK (i.e. 25 ºC) (micromol*s-1*m-2)
     * @param vmax298 - maximum Rubisco carboxylation rate per leaf area at 298 ºK (i.e. 25 ºC) (micromol*s-1*m-2)
     */
    void Update_photosynthesis(double ppfd, double catm, double gs, double tleaf, bool verbose=false);
    double Update_gs(double beta, double an, double ca_ppm, double vpd);

    void Solve_Anet_gs(double ppfd, double catm, double vpd, double tleaf, double beta);

    double Get_An(){
        return An;
    }

    double Get_Gs(){
        return gs;
    }


private:
    const Parameters& parameters;

    // micromol*s-1*m-2
    double An;
    // umol mol1 or ppm
    double ci;
    // mol m-2 s-1
    double gs;


    // Constants
    const double R_gas = 8.314; //(J/mol/ºK) Universal gas constant
    const double O2_conc = 209.0; //mmol*mol-1 (Collatz et al. 2001)
    const double quantumYield = 0.3; //mol photon * mol-1 electron
    const double lightResponseCurvature = 0.9;

    double gammaTemp(double Tleaf);
    double KmTemp(double Tleaf, double Oi);
    double KoTemp(double Tleaf);
    double KcTemp(double Tleaf);

    double VmaxTemp(double Vmax298, double Tleaf);
    double JmaxTemp(double Jmax298, double Tleaf);

    double fder(double x, double Q, double Ca, double Gc, double GT, double Km, double Vmax, double Jmax);
    double f(double x, double Q, double Ca, double Gc, double GT, double Km, double Vmax, double Jmax);

    double photosynthesis_Ci(double Q, double Ci, double GT, double Km, double Vmax, double Jmax);

    double electronLimitedPhotosynthesisDerivative(double Q, double Ci, double GT, double Jmax);
    double electronLimitedPhotosynthesis(double Q, double Ci, double GT, double Jmax);

    double rubiscoLimitedPhotosynthesis(double Ci, double GT, double Km, double Vmax);
    double rubiscoLimitedPhotosynthesisDerivative(double Ci, double GT, double Km, double Vmax);



};


#endif //SIMPHONY_ASSIMI_FARQUQR_H
