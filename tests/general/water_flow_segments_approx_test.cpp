//
// Created by Phillip on 22.04.24.
//
#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/modules/stem_flow_model.h"
#include <iostream>

TEST(General_tests, Stem_flow_model_tests)
{

    const double EPS  = 1E-6;

    Parameters parameters;
    std::vector<std::unique_ptr<Stem_flow_module> > modules;

    parameters.sustain_xylem_damage = false;
    parameters.psi50_xylem = -3.8;
    parameters.psi88_xylem= -5.5;

    parameters.k_xylem_sat = 1.0;
    parameters.huber_value = 1.0;
    parameters.canopy_height = 1.0;

    modules.push_back(std::make_unique<Kirchhoff_Weibull_stem_flow>(parameters));
    modules.push_back(std::make_unique<Linear_Segmented_flow>(parameters));

    for (auto& module: modules) {
        module->Init();
    }

    double psi_leaf = -4.5;
    double psi_stem = -1.5;

    for (auto& module: modules) {
        std::cout << module->Get_Stem_flow(psi_stem, psi_leaf) << std::endl;
    }

    std::cout << "Testing Kirchhof waterflow integral vs. Mathematica implementation...";
    double mathematica_value = 2.0490506751287496;
    ASSERT_NEAR(mathematica_value, modules[0]->Get_Stem_flow(psi_stem, psi_leaf), EPS);

    std::cout << "Testing Linear waterflow approximation vs. Mathematica implementation..";
    mathematica_value = 2.049664469676958;
    ASSERT_NEAR(mathematica_value, modules[1]->Get_Stem_flow(psi_stem, psi_leaf), EPS);

    double max_error = 0.0;
    double psi_leaf_error = 0.0;
    double psi_stem_error = 0.0;

    std::cout << "Testing Linear vs. Kirchoff implementation..";
    for (double psi_stem_r = -6.0; psi_stem_r < -0.1 ; psi_stem_r += 0.1) {
        for (double psi_leaf_r = -10.0; psi_leaf_r < -0.1 ; psi_leaf_r += 0.1) {

            if (psi_leaf_r < psi_stem_r){
                double x = modules[0]->Get_Stem_flow(psi_stem_r, psi_leaf_r);
                double y = modules[1]->Get_Stem_flow(psi_stem_r, psi_leaf_r);

                if (max_error < std::abs(x - y) / y){
                    max_error = std::abs(x - y) / y;
                    psi_leaf_error = psi_leaf_r;
                    psi_stem_error = psi_stem_r;
                }
            }
        }
    }

    if (max_error < 1E-1){
        std::cout << "Pass! (Relative deviation: " << max_error << ")" << std::endl;
        std::cout << "Psi leaf: " << psi_leaf_error << std::endl;
        std::cout << "Psi stem: " << psi_stem_error << std::endl;
    }
    else{
        std::cout << "Failed (Relative error: " << max_error << ")" << std::endl;
        std::cout << "Psi leaf: " << psi_leaf_error << std::endl;
        std::cout << "Psi stem: " << psi_stem_error << std::endl;
    }
    ASSERT_TRUE(max_error < 1E-1);



}
