#include "parameters.h"
#include "../modules/root_distribution_model.h"
#include <iostream>

Parameters::Parameters() {
}

Parameters::~Parameters() {

}

void Parameters::Set_derived() {

    size_t n_soil_layers =  soil_layers.size();

//    for (int i = 0; i < n_soil_layers; ++i) {
//        soil_layers[i].depth = soil_depths[i];
//    }
//
//    if (organic_matter_fracs.size() != n_soil_layers){
//        std::cout << "OM_fracs are differnt in size.." << std::endl;
//        exit(99);
//    }
//    for (int i = 0; i < n_soil_layers; ++i) {
//        soil_layers[i].organic_matter_fraction = organic_matter_fracs[i];
//    }
//
//    if (sand_fracs.size() != n_soil_layers){
//        std::cout << "Sand_fracs are differnt in size.." << std::endl;
//        exit(99);
//    }
//    for (int i = 0; i < n_soil_layers; ++i) {
//        soil_layers[i].sand_fraction = sand_fracs[i];
//    }
//
//    if (clay_fracs.size() != n_soil_layers){
//        std::cout << "Clay_fracs are differnt in size.." << std::endl;
//        exit(99);
//    }
//    for (int i = 0; i < n_soil_layers; ++i) {
//        soil_layers[i].clay_fraction = clay_fracs[i];
//    }
//
//    if (k_soil_sats.size() != n_soil_layers){
//        std::cout << "k_soil sats are differnt in size.." << std::endl;
//        exit(99);
//    }
//    for (int i = 0; i < n_soil_layers; ++i) {
//        soil_layers[i].k_soil_sat = k_soil_sats[i];
//    }
    Root_distribution_model root_model(*this);
    // Rooting fraction per laye [-]
    std::vector<double> root_fraction_player = root_model.Get_root_fractions();
    // Soil layer depths accumulated
    std::vector<double> soil_layer_depth_acc  = root_model.Get_soil_layer_depth_acc();

    for (int i = 0; i < n_soil_layers; ++i) {
        soil_layers[i].root_fraction = root_fraction_player[i];
    }

}


