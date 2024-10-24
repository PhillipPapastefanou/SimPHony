//
// Created by Phillip on 04/09/2024.
//

#include "gtest/gtest.h"
#include "../../src/core/cpp/framework/parameters.h"
#include "../../src/core/cpp/io/input_hainich.h"
#include "../../src/core/cpp/modules/soil_water/campbell.h"
#include "../../src/core/cpp/modules/soil_water/van_genuchten.h"
#include "../../src/core/cpp/modules/soil_water/saxton.h"
#include "../../src/core/cpp/modules/soil_water/soil_water_model.h"
#include <iostream>


TEST(General_tests, Soil_model_tests)
{
    std::cout << "Performing General_tests.Soil_model_tests..." << std::endl;
    Parameters parameters;
    Input_Hainich input = Input_Hainich();

    parameters.soil_layers.resize(1);
    parameters.soil_layers[0].k_soil_sat = 9.8084e-06;
    parameters.soil_layers[0].sand_fraction = 0.10;
    parameters.soil_layers[0].clay_fraction = 0.3;
    parameters.soil_layers[0].organic_matter_fraction = 0.05;

    Soil_layer& layer = parameters.soil_layers[0];

    layer.psi_soil_sat = -0.31655;
    layer.theta_s = 0.42685;
    layer.theta_r = 0.033675;
    layer.camp_b = 5.4763;
    layer.pore_size_ind = 0.24985;

    const double eps = 1E-5;
    std::vector<std::vector<float> > thetas;

    for (int i = 0; i < 2; ++i) {

        std::vector<float> theta(1);
        theta[0] = 0.05 + i*0.3;
        thetas.push_back(theta);
    }

    input.theta_per_layer = thetas;
    Campbell camp(parameters, input);
    camp.CalculatePsiAndKs();

    vector<vector<double>> ks = camp.Get_ks();
    vector<vector<double>> phead = camp.Get_psi_soil_head();

    vector<double> ks_c  = {9.941870e-19, 6.148696e-07};
    std::cout << "Campell ks 1 ";
    ASSERT_NEAR(ks_c[1], ks[1][0], eps);

    std::cout << "Campell ks 0 ";
    ASSERT_NEAR(ks_c[0], ks[0][0], eps);

    vector<double> phead_obs  = {-39861.113883,
                                 -0.938726};
    std::cout << "Campell head 1 ";
    ASSERT_NEAR(phead_obs[1], phead[1][0], eps *10000.0);

    std::cout << "Campell head 0 ";
    ASSERT_NEAR(phead_obs[0], phead[0][0], eps*10000.0);


    // Testing Van Genuchten

    Van_Genuchten vng(parameters, input);
    vng.CalculatePsiAndKs();

    ks = vng.Get_ks();
    phead = vng.Get_psi_soil_head();

    ks_c  = {1.197875e-21, 5.469397e-08
    };
    std::cout << "VGN ks 1 ";
    ASSERT_NEAR(ks_c[1], ks[1][0], eps);

    std::cout << "VGN ks 0 ";
    ASSERT_NEAR(ks_c[0], ks[0][0], eps);

    phead_obs  = {-107322.615168,
                  -0.544152};
    std::cout << "VGN head 1 ";
    ASSERT_NEAR(phead_obs[1], phead[1][0], eps*10000.0);

    std::cout << "VGN head 0 ";
    ASSERT_NEAR(phead_obs[0], phead[0][0], eps*10000.0);

    // Comparing all three

    std::vector<std::unique_ptr<Soil_water_module> > models;

    models.push_back(std::make_unique<Van_Genuchten>(parameters,input));
    models.push_back(std::make_unique<Campbell>(parameters,input));
    models.push_back(std::make_unique<Saxton06>(parameters,input));

    for (auto& model: models){
        model->CalculatePsiAndKs();
    }

    for (auto& model: models){
        auto kss = model->Get_ks();
        std::cout << kss[0][0] << std::endl;
    }

    std::cout << "---------" << std::endl;

    for (auto& model: models){
        auto kss = model->Get_ks();
        std::cout << kss[1][0] << std::endl;
    }
    std::cout << "---------" << std::endl;

    for (auto& model: models){
        auto head = model->Get_psi_soil_head();
        std::cout << head[0][0] << std::endl;
    }
    std::cout << "---------" << std::endl;

    for (auto& model: models){
        auto head = model->Get_psi_soil_head();
        std::cout << head[1][0] << std::endl;
    }

    std::cout << std::string(64, '-') << std::endl;
    std::cout << std::endl;

}
