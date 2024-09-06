//
// Created by Phillip on 04/09/2024.
//

#include "soil_models_test.h"
#include "../src/framework/parameters.h"
#include "../src/io/input_hainich.h"
#include "../src/modules/soil_water/campbell.h"
#include "../src/modules/soil_water/van_genuchten.h"
#include "../src/modules/soil_water/saxton.h"
#include "../src/modules/soil_water/soil_water_model.h"
#include <iostream>


void ASSERT(double obs, double mod, double max_rel_error){

    const double rel_error = std::abs((obs - mod)/obs);

    if(rel_error < max_rel_error ){
        std::cout << "Passed!" << std::endl;

    }
    else{
        std::cout << "Failed!" << std::endl;
    }
}

Soil_Models_Test::Soil_Models_Test() {

    Parameters parameters;
    Input_Hainich input = Input_Hainich(parameters);

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


    double eps = 1E-5;

    std::vector<std::vector<float> > thetas;

    for (int i = 0; i < 2; ++i) {

        std::vector<float> theta(1);
        theta[0] = 0.05 + i*0.3;
        thetas.push_back(theta);
    }

    input.theta_per_layer = thetas;


    // Testing campbell

    Campbell camp(parameters, input);
    camp.CalculatePsiAndKs();

    vector<vector<double>> ks = camp.Get_ks();
    vector<vector<double>> phead = camp.Get_psi_head();

    vector<double> ks_c  = {9.941870e-19, 6.148696e-07 };
    std::cout << "Campell ks 1 ";
    ASSERT(ks_c[1], ks[1][0], eps);

    std::cout << "Campell ks 0 ";
    ASSERT(ks_c[0], ks[0][0], eps);

    vector<double> phead_obs  = {-39861.113883,
                                 -0.938726};
    std::cout << "Campell head 1 ";
    ASSERT(phead_obs[1], phead[1][0], eps);

    std::cout << "Campell head 0 ";
    ASSERT(phead_obs[0], phead[0][0], eps);


    // Testing Van Genuchten

    Van_Genuchten vng(parameters, input);
    vng.CalculatePsiAndKs();

    ks = vng.Get_ks();
    phead = vng.Get_psi_head();

    ks_c  = {1.197875e-21, 5.469397e-08
    };
    std::cout << "VGN ks 1 ";
    ASSERT(ks_c[1], ks[1][0], eps);

    std::cout << "VGN ks 0 ";
    ASSERT(ks_c[0], ks[0][0], eps);

    phead_obs  = {-107322.615168,
                  -0.544152};
    std::cout << "VGN head 1 ";
    ASSERT(phead_obs[1], phead[1][0], eps);

    std::cout << "VGN head 0 ";
    ASSERT(phead_obs[0], phead[0][0], eps);

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
        auto head = model->Get_psi_head();
        std::cout << head[0][0] << std::endl;
    }
    std::cout << "---------" << std::endl;

    for (auto& model: models){
        auto head = model->Get_psi_head();
        std::cout << head[1][0] << std::endl;
    }
    std::cout << "---------" << std::endl;



}
