//
// Created by Phillip on 11/03/2025.
//
//
// Created by Phillip on 04/09/2024.
//
#include <iostream>
#include "gtest/gtest.h"
#include "../../../src/core/cpp/framework/parameters.h"
#include "../../../src/core/cpp/modules/phot/assimilation.h"


TEST(General_tests, Assimilation)
{

    std::cout << "Performing photoynthesis tests " << std::endl;
    Parameters parameters;

    Assimilation assimilation(parameters);

    // Precision
    const double EPS  = 2E-5;
    // Air temperature
    double tair = 20.0;
    // Shortwave downward radiation [W m-2]
    double sw_rad = 300;
    // gs/Anet [-]
    double gs_over_anet = 0.001;
    // Ambient co2 concentration [ppm]
    double ca_ppm = 400;

    double ca_pa = ca_ppm * parameters.constants.PPM_TO_PA;
    assimilation.Update_photosythesis(tair, sw_rad, ca_pa, gs_over_anet);

    double anet = assimilation.Get_An();
    double anet_ref = 5.649736556367912e-07;
    ASSERT_NEAR(anet,anet_ref, EPS );

    gs_over_anet = 0.01;
    assimilation.Update_photosythesis(tair, sw_rad, ca_pa, gs_over_anet);
    anet = assimilation.Get_An();
    anet_ref = 24.082121267852628;
    ASSERT_NEAR(anet,anet_ref, EPS );

    std::cout << std::string(64, '-') << std::endl;
    std::cout << "Testing stomatal conductance.. " <<  std::endl;

    const double PA_TO_PPM = 1000.0 / 101.325;


    // Initialise ci_ppm
    double ci_ppm = ca_pa * PA_TO_PPM;

    parameters.g0 = 0.01;
    parameters.g1 =  2.35;

    // kPa
    double vpd = 1.0;
    double beta = 1.0;
    anet = 10.0;
    gs_over_anet =   assimilation.Update_gs_over_anet(beta, anet, ci_ppm, vpd);

    double gs_over_anet_ref = 0.009374917345992145;
    ASSERT_NEAR(gs_over_anet, gs_over_anet_ref, EPS);
    std::cout << std::string(64, '-') << std::endl;
    std::cout << "Testing gs over anet (beta = 1.0) " <<  std::endl;
    assimilation.Solve_Anet_gs(vpd, tair, beta, sw_rad, ca_ppm);
    anet = assimilation.Get_An();
    double gs = assimilation.Get_Gs();
    anet_ref = 23.737574090862893 ;
    double gs_ref = 0.3526295831470652;
    ASSERT_NEAR(gs, gs_ref, EPS);
    ASSERT_NEAR(anet, anet_ref, EPS);
    std::cout << "Testing gs over anet (beta = 0.0) " <<  std::endl;
    beta  = 0.0;
    assimilation.Solve_Anet_gs(vpd, tair, beta, sw_rad, ca_ppm);
    anet = assimilation.Get_An();
    gs = assimilation.Get_Gs();
    anet_ref = 9.94649911149937;
    gs_ref= 0.055982696965122015;
    ASSERT_NEAR(gs, gs_ref, EPS);
    ASSERT_NEAR(anet, anet_ref, EPS);

    std::cout << "Testing gs over anet (beta = 0.0) and high vpd" <<  std::endl;
    beta  = 0.0;
    vpd = 5.0;
    assimilation.Solve_Anet_gs(vpd, tair, beta, sw_rad, ca_ppm);
    anet = assimilation.Get_An();
    gs = assimilation.Get_Gs();
    anet_ref = 9.94649911149937;
    gs_ref= 0.055982696965122015;
    ASSERT_NEAR(gs, gs_ref, EPS);
    ASSERT_NEAR(anet, anet_ref, EPS);


    std::cout << "Testing gs over anet (beta = 0.5) and high vpd" <<  std::endl;
    beta  = 0.5;
    vpd = 5.0;
    assimilation.Solve_Anet_gs(vpd, tair, beta, sw_rad, ca_ppm);
    anet = assimilation.Get_An();
    gs = assimilation.Get_Gs();
    anet_ref = 15.87417877806658;
    gs_ref= 0.11566899258819092;
    ASSERT_NEAR(gs, gs_ref, EPS);
    ASSERT_NEAR(anet, anet_ref, EPS);

    std::cout << "Testing gs over anet (beta = 1.0) and low vpd (0.0)" <<  std::endl;
    beta  = 1.0;
    vpd = 0.0;
    assimilation.Solve_Anet_gs(vpd, tair, beta, sw_rad, ca_ppm);
    anet = assimilation.Get_An();
    gs = assimilation.Get_Gs();
    anet_ref = 27.86198850335728;
    gs_ref = 1.3934426422616866;
    ASSERT_NEAR(gs, gs_ref, EPS);
    ASSERT_NEAR(anet, anet_ref, EPS);


}
