//
// Created by Phillip on 27.04.24.
//

#pragma once

class RKF_Solver {

public:


    void calc();

    double dpsi(double psi);




private:

    double update();

    double psi;

    double h;

    double psi4;


    double tolerance;


    /// constants from Fehlberg 1970
    const double a21 = 1.0/4.0;

    const double a31 = 3.0/32.0;
    const double a32 = 9.0/32.0;

    const double a41 = 1932.0/2197.0;
    const double a42 = - 7200.0/2197.0;
    const double a43 = 7296.0/2197.0;

    const double a51 = 439.0/216.0;
    const double a52 = -8.0;
    const double a53 = 3680.0/513.0;
    const double a54 = - 845.0/4104.0;

    const double a61 = - 8.0/27.0;
    const double a62 = 2.0;
    const double a63 = - 3544.0/2565.0;
    const double a64 = 1859.0/4104.0;
    const double a65 = - 11.0/40.0;

    const double d1 = 25.0 / 216.0;
    const double d2 = 0.0;
    const double d3 = 1408.0/2565.0;
    const double d4 = 2197.0/4101.0;
    const double d5 = -1.0/5.0;

    const double e1 = 16.0 / 135.0;
    const double e2 = 0.0;
    const double e3 = 6656.0 / 12825.0;
    const double e4 = 28561.0 / 56430.0;
    const double e5 = -9.0/50.0;
    const double e6 = 2.0/55.0;

};
