//
// Created by Phillip on 11/03/2025.
//
//
// Created by Phillip on 04/09/2024.
//
#include <iostream>
#include "gtest/gtest.h"
#include "../../../src/core/cpp/framework/parameters.h"
#include "../../../src/core/cpp/modules/phot/assimi_farquar.h"


TEST(General_tests, Assimilation_Farquar_phot)
{

    std::cout << "Performing photoynthesis tests " << std::endl;
    Parameters parameters;

    Assimi_Farquar assimilation(parameters);

    // Precision
    const double EPS  = 2E-5;
    // Air temperature
    double tair = 20.0;
    // Shortwave downward radiation [W m-2]
    double sw_rad = 300;
    // Ambient co2 concentration [ppm]
    double ca_ppm = 400;

    //# SW (W m-2) to PAR (umol m-2 s-1)
    const double SW_2_PAR = 4.57 * 0.5;

    double ppfd = sw_rad * SW_2_PAR;

    double gs = 0.001;
    double vmax25 = 43;
    double jmax = 71;

    assimilation.Update_photosynthesis(ppfd, ca_ppm, gs, tair, vmax25, jmax);

}


TEST(General_tests, Assimilation_Farquar_gs_phot)
{

    std::cout << "Performing photoynthesis tests " << std::endl;
    Parameters parameters;

    Assimi_Farquar assimilation(parameters);

    // Precision
    const double EPS  = 2E-5;
    // Air temperature
    double tair = 30.0;
    // Shortwave downward radiation [W m-2]
    double sw_rad = 100;
    // Ambient co2 concentration [ppm]
    double ca_ppm = 400;

    //# SW (W m-2) to PAR (umol m-2 s-1)
    const double SW_2_PAR = 4.57 * 0.5;

    double ppfd = sw_rad * SW_2_PAR;

    double vmax25 = 43;
    double jmax = 71;
    double beta = 1.0;
    double an = 1;
    double vpd = 1;


    double gs = assimilation.Update_gs(beta, an, ca_ppm, vpd);

    for (int i = 0; i < 500; i=i+10) {
        ppfd = i * SW_2_PAR;
        assimilation.Solve_Anet_gs(ppfd,ca_ppm,vpd,tair,vmax25, jmax,beta);

        std::cout << i <<  " " << assimilation.Get_Gs() << std::endl;

    }



}
