//
// Created by Phillip on 27.04.24.
//

#include "rkf_solver.h"
#include <cmath>
#include <iostream>

double RKF_Solver::dpsi(double psin) {
   // return 0.001 * psin * std::sin(psin)/(psin+0.002);
    return 0.001 * psin +  std::exp(-psin);
}

double RKF_Solver::update() {


    double psin = psi;
    const double k1 = h * dpsi(psin);
    psin = psi + a21 * k1;
    const double k2 = h * dpsi(psin);
    psin = psi + a31 * k1 + a32 * k2;
    const double k3 = h * dpsi(psin);
    psin = psi + a41 * k1 + a42 * k2 + a43 * k3;
    const double k4 = h * dpsi(psin);
    psin = psi + a51 * k1 + a52 * k2 + a53 * k3 + a54 * k4;
    const double k5 = h * dpsi(psin);
    psin = psi + a61 * k1 + a62 * k2 + a63 * k3 + a64 * k4 + a65 * k5;
    const double k6 = h * dpsi(psin);

    psi4 = psi
                 + d1 * k1
                 + d3 * k3
                 + d4 * k4
                 + d5 * k5;

    const double psi_5 = psi
                              + e1 * k1
                              + e3 * k3
                              + e4 * k4
                              + e5 * k5
                              + e6 * k6;

    double frac_step_psi_leaf = std::pow((tolerance * h) / (2.0 * std::abs(psi_5 - psi4)), 1.0 / 4.0);

    return h*frac_step_psi_leaf;

}

void RKF_Solver::calc() {

    psi = 1;

    tolerance = 1e-8;
    double dts = 1800;
    double dts_remain = dts;
    h = dts/100;

    while(dts_remain > 0){

        double h_new = update();

        h = h_new;

        if (h < dts/1000){
            h = dts/1000;
        }

        if(h > dts_remain){
            h = dts_remain;
        }

        update();

        psi = psi4;

        //std::cout << psi << std::endl;

        dts_remain-= h;
    }

    std::cout << psi << std::endl;

}
