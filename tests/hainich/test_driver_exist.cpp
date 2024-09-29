//
// Created by Phillip on 26/09/2024.
//

#include <filesystem>
#include "gtest/gtest.h"
TEST(Hainich_tests, Stem_flow_model_tests) {

    using std::string;
    string forcing_file = "../data/hainich/input/Meteo_Hainich_dT30min_forcing_PHS.csv";
    string sap_file = "../data/hainich/eval/SAP_Hainich_Fagus-mean_dT30min_prog.csv";
    string psi_stem_file = "../data/hainich/eval/stem_water_pot.csv";

    bool forcing_file_exists = std::filesystem::exists(forcing_file);
    ASSERT_TRUE(forcing_file_exists);

    bool sap_file_exists = std::filesystem::exists(sap_file);
    ASSERT_TRUE(sap_file_exists);

    bool psi_stem_file_exists = std::filesystem::exists(psi_stem_file);
    ASSERT_TRUE(psi_stem_file_exists);
}