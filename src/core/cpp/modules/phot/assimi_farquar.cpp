//
// Created by Phillip on 13/03/2025.
//

#include "assimi_farquar.h"
#include <cmath>
#include <iostream>

//Michaelis-Menten coefficients of Rubisco for Carbon (micromol * mol-1)
double  Assimi_Farquar::KcTemp(double Tleaf) {return(404.9*exp((79430*(Tleaf-25.0))/(298.0*R_gas*(Tleaf+273))));}

//Michaelis-Menten coefficients of Rubisco for Oxigen (mmol * mol-1)
double  Assimi_Farquar::KoTemp(double Tleaf) {return(278.4*exp((36380*(Tleaf-25.0))/(298.0*R_gas*(Tleaf+273))));}

double  Assimi_Farquar::KmTemp(double Tleaf, double Oi = 209.0) {
    double Kc = KcTemp(Tleaf);
    double Ko = KoTemp(Tleaf);
    return(Kc*(1.0+(Oi/Ko)));
}

double Assimi_Farquar::gammaTemp(double Tleaf) {return(42.75*exp((37830*(Tleaf-25.0))/(298.0*R_gas*(Tleaf+273))));}

double Assimi_Farquar::VmaxTemp(double Vmax298, double Tleaf) {
    double Ha = 73637.0; //Energy of activation J * mol-1
    double Hd = 149252.0; //Energy of deactivation J * mol-1
    double Sv = 486.0;  //Entropy term J * mol-1 * K-1
    double C = 1.0+exp((Sv*298.2-Hd)/(R_gas*298.2));
    return(Vmax298*(C*exp((Ha/(R_gas*298.2))*(1.0-298.2/(Tleaf+273.2))))/(1.0+exp((Sv*Tleaf-Hd)/(R_gas*(Tleaf+273.2)))));
}


double Assimi_Farquar::JmaxTemp(double Jmax298, double Tleaf) {
    double Ha = 50300.0; //Energy of activation J * mol-1
    double Hd = 152044.0; //Energy of deactivation J * mol-1
    double Sv = 495.0;  //Entropy term J * mol-1 * K-1
    double C = 1.0+exp((Sv*298.2-Hd)/(R_gas*298.2));
    return(Jmax298*(C*exp((Ha/(R_gas*298.2))*(1.0-298.2/(Tleaf+273.2))))/(1.0+exp((Sv*Tleaf-Hd)/(R_gas*(Tleaf+273.2)))));
}


// Auxiliary functions for Newton-Raphson
double Assimi_Farquar::f(double x, double Q, double Ca, double Gc, double GT, double Km, double Vmax, double Jmax) {
    return(photosynthesis_Ci(Q,x, GT, Km, Vmax, Jmax)-(Gc*(Ca-x)));
}

double Assimi_Farquar::fder(double x, double Q, double Ca, double Gc, double GT, double Km, double Vmax, double Jmax) {
    double Je = electronLimitedPhotosynthesis(Q, x, GT, Jmax);
    double dJe = electronLimitedPhotosynthesisDerivative(Q, x, GT, Jmax);
    double Jc = rubiscoLimitedPhotosynthesis(x, GT, Km, Vmax);
    double dJc = rubiscoLimitedPhotosynthesisDerivative(x, GT, Km, Vmax);
    double dA1 = (1.0/(2.0*0.98))*(dJe+dJc-(0.5*pow(pow(Je+Jc,2.0)-4.0*0.98*Je*Jc,-0.5)*(2.0*Je*dJe+2.0*Jc*dJc+(2.0-4.0*0.98)*(dJe*Jc + dJc*Je))));
    double dA2 = -Gc;
    return(dA1-dA2);
}

/**
 * Calculates photosynthesis (Farquhar et al. 1980/Collatz et al 1991)
 *
 * Ci - CO2 internal concentration (micromol * mol-1)
 * GT - CO2 saturation point corrected by temperature (micromol * mol-1)
 * Km = Kc*(1.0+(Oi/Ko)) - Michaelis-Menten term corrected by temperature (in micromol * mol-1)
 * Q - Active photon flux density (micromol * s-1 * m-2)
 * Jmax - maximum electron transport rate per leaf area (micromol*s-1*m-2) corrected  by temperature
 * Vmax - maximum Rubisco carboxylation rate per leaf area (micromol*s-1*m-2) corrected  by temperature
 *
 * return units: micromol*s-1*m-2
 */
double Assimi_Farquar::photosynthesis_Ci(double Q, double Ci, double GT, double Km, double Vmax, double Jmax) {
    double Je = electronLimitedPhotosynthesis(Q, Ci, GT, Jmax);
    double Jc = rubiscoLimitedPhotosynthesis(Ci, GT, Km, Vmax);
    return(std::max(0.0,(Je+Jc-sqrt(pow(Je+Jc,2.0)-4.0*0.98*Je*Jc))/(2.0*0.98)));
}

/**
 * Calculates electron-limited photosynthesis (Farquhar et al. 1980)
 *
 * Ci - CO2 internal concentration (micromol * mol-1)
 * GT - CO2 saturation point corrected by temperature (micromol * mol-1)
 * Q - Active photon flux density (micromol * s-1 * m-2)
 * Jmax - maximum electron transport rate per leaf area (micromol*s-1*m-2)
 *
 * return units: micromol*s-1*m-2
 */
//' @rdname photo
//' @keywords internal
// [[Rcpp::export("photo_electronLimitedPhotosynthesis")]]
double Assimi_Farquar::electronLimitedPhotosynthesis(double Q, double Ci, double GT, double Jmax) {
    double J = ((quantumYield*Q+Jmax)-sqrt(pow(quantumYield*Q+Jmax, 2.0) - 4.0*lightResponseCurvature*quantumYield*Q*Jmax))/(2.0*lightResponseCurvature);
    return((J/4.0)*((Ci-GT)/(Ci+2.0*GT)));
}
double Assimi_Farquar::electronLimitedPhotosynthesisDerivative(double Q, double Ci, double GT, double Jmax){
    double J = ((quantumYield*Q+Jmax)-sqrt(pow(quantumYield*Q+Jmax, 2.0) - 4.0*lightResponseCurvature*quantumYield*Q*Jmax))/(2.0*lightResponseCurvature);
    return((J/4.0)*((3.0*GT)/pow(Ci+2.0*GT,2.0)));
}


/**
 * Calculates rubisco-limited photosynthesis (Farquhar et al. 1980)
 *
 * Ci - CO2 internal concentration (micromol * mol-1)
 * GT - CO2 saturation point corrected by temperature (micromol * mol-1)
 * Km = Kc*(1.0+(Oi/Ko)) - Michaelis-Menten term corrected by temperature (in micromol * mol-1)
 * Vmax - maximum Rubisco carboxylation rate per leaf area (micromol*s-1*m-2)
 *
 * return units: micromol*s-1*m-2
 */
//' @rdname photo
//' @keywords internal
// [[Rcpp::export("photo_rubiscoLimitedPhotosynthesis")]]
double Assimi_Farquar::rubiscoLimitedPhotosynthesis(double Ci, double GT, double Km, double Vmax) {
    return(Vmax *(Ci-GT)/(Ci+Km));
}
double Assimi_Farquar::rubiscoLimitedPhotosynthesisDerivative(double Ci, double GT, double Km, double Vmax) {
    return(Vmax *(Km+GT)/pow(Ci+Km,2.0));
}


void Assimi_Farquar::Update_photosynthesis(double ppfd, double catm, double gs, double tleaf)
                                            {
    double GT = gammaTemp(tleaf);
    double Km = KmTemp(tleaf, O2_conc);
    double Vmax = VmaxTemp(parameters.vmax25, tleaf);
    double Jmax = JmaxTemp(parameters.jmax25, tleaf);
    double x,x1,e,fx,fx1;
    x1 = 0.0;//initial guess
    e = 0.001; // accuracy in micromol * mol-1
    int cnt = 0;
    int mxiter = 100;
    do {
        x=x1; /*make x equal to the last calculated value of                             x1*/
        fx=f(x, ppfd, catm, gs, GT, Km, Vmax, Jmax);            //simplifying f(x)to fx
        fx1=fder(x, ppfd, catm, gs, GT, Km, Vmax, Jmax);            //simplifying fprime(x) to fx1
        x1=x-(fx/fx1);/*calculate x{1} from x, fx and fx1*/
        cnt++;
    } while ((std::abs(x1-x)>=e) && (cnt < mxiter));
    An = photosynthesis_Ci(ppfd,x1,GT,Km,Vmax,Jmax);
    ci = x1;
}

double Assimi_Farquar::Update_gs(double beta, double an, double ca_ppm, double vpd) {
    if (vpd < 0.05)
        vpd = 0.05;
    return parameters.g0 + (1.0 + beta * parameters.g1 / std::sqrt(vpd))* (an / ca_ppm);
}

Assimi_Farquar::Assimi_Farquar(const Parameters &parameters): parameters(parameters) {

}

void Assimi_Farquar::Solve_Anet_gs(double ppfd, double catm, double vpd, double tleaf,
                                   double beta) {
    double gs_prev = 0.0;
    gs = 0.1;
    bool converged = false;
    const double EPS = 1E-6;
    int MAXITER = 100;
    for (int i = 0; i < MAXITER; ++i) {
        Update_photosynthesis(ppfd, catm, gs, tleaf);
        gs_prev = gs;
        gs = Update_gs(beta, An, catm, vpd);
        if (std::abs(gs-gs_prev) < EPS){
            converged = true;
            break;
        }
    }

    if (!converged){
        std::cout << "Assimilation routine did not converge" << std::endl;
    }
    


}
