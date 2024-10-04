//
// Created by Phillip on 04/09/2024.
//

#include "gtest/gtest.h"
#include "../../src/core/cpp/auxil/amath.h"
#include <iostream>


TEST(General_tests, Special_functions_test){

    const double EPS = 1E-9;

    double approx, exact;
    std::cout << "Testing ExpIntergral... ";

    approx = expint(1.0, 4.0);
    exact = 0.003779352409848905;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(0, 1.0);
    exact = 0.3678794411714423;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(2, 3.0);
    exact = 0.010641925085273;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(3.0, 6.5);
    exact = 0.0001629981562939636;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(2.5, 6.5);
    exact = 0.0001716254200396746;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(6.5, 6.5);
    exact = 0.0001199223866978655;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(6.5, 6.5);
    exact = 0.0001199223866978655;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(7.0, 0.2);
    exact = 0.1312532009281553;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(10.0, 10.0);
    exact = 0.000002325302657028204;
    ASSERT_NEAR(approx, exact, EPS);

    approx = expint(8.9, 1.6);
    exact = 0.02085725242618081;
    ASSERT_NEAR(approx, exact, EPS);
}



