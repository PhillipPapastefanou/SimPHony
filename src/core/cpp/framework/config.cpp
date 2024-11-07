//
// Created by Phillip on 30/10/2024.
//
#include "../auxil/csv_reader.h"
#include <vector>
#include "config.h"
#include <algorithm>
#include <fstream>

Config::Config():
        forcing_file("forcing_file"),
        sap_flow_file("sap_flow_file"),
        psi_stem_file("psi_stem_file"),
        soilwater_file("soilwater_file"),
        swiss_tree_folder_path("swiss_tree_folder_path"),
        swiss_soil_water_input_type("swiss_soil_water_input_type"),
        parameters_list_file("parameters_list_file"){
}

Config::~Config() {
}

void Config::Read(std::string filename) {

    io::CSV_Reader reader(filename, false, '=');

    std::vector<std::vector<std::string> > data = reader.Get<string>();

    for (int i = 0; i < data.size(); ++i) {

        std::string key = data[i][0];

        // If we have no value skip the row
        if (data[i].size() <2)
            continue;
        std::string value = data[i][1];

        // Remove whitespaces
        value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());

        if (key[0] == '%'){
            continue;
        }




        if (key == "location"){
            if (value == "Hainich"){
                location = Location::Hainich;
            }
            else if(value == "Swiss_cc"){
                location = Location::Swiss_cc;
            }
            else{
                std::cout << "Invalid location specified!" << std::endl;
                exit(99);
            }
        }

        Parse(forcing_file, key, value);
        Parse(parameters_list_file, key, value);

        if (location == Location::Swiss_cc) {
            Parse(soilwater_file, key, value);
            Parse(swiss_tree_folder_path, key, value);

            if(key == swiss_soil_water_input_type.key){

                std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c){ return std::tolower(c); });

                if(value == "nlayers_mean")
                    swiss_soil_water_input_type.value = Swiss_soil_water_input_type::NLayersMean;
                else if(value == "nlayers_mean_one_std")
                    swiss_soil_water_input_type.value = Swiss_soil_water_input_type::NLayersMeanOneStd;
                else if(value == "nlayers_mean_n_std")
                    swiss_soil_water_input_type.value = Swiss_soil_water_input_type::NLayersMeanNStd;
                else if(value == "nlayers_indiv")
                    swiss_soil_water_input_type.value = Swiss_soil_water_input_type::NLayersIndiv;
                else{
                    std::cout << "Invalid swiss_soil_water_input_type" << std::endl;
                    exit(99);
                }
                swiss_soil_water_input_type.found = true;
            }


        }
        else if (location == Location::Hainich){
            Parse(sap_flow_file, key, value);
            Parse(psi_stem_file, key, value);
        }
    }

    // Check whether all files require have been found

    Check(forcing_file);

    if (location == Location::Swiss_cc) {
        Check(soilwater_file);
        Check(swiss_tree_folder_path);
        Check(swiss_soil_water_input_type);
    }
    if (location == Location::Hainich){
        Check(sap_flow_file);
        Check(psi_stem_file);
    }

}

void Config::Create_swiss_cc() {
    location = Location::Swiss_cc;
    forcing_file.value = "../data/swiss/input/Forcing_Inter.csv";
    swiss_tree_folder_path.value = "../data/swiss/eval/Trees";
    parameters_list_file.value = "../tests/swiss_cc/test/input/parameter_example_1.csv";
    soilwater_file.value =  "../data/swiss/input/vwc_swicc_cc_2023_std_n.csv";
    swiss_soil_water_input_type.value = Swiss_soil_water_input_type::NLayersMeanNStd;
}


void Config::Create_hainich() {

    location = Location::Hainich;
    forcing_file.value = "../data/hainich/input/Meteo_Hainich_dT30min_forcing_PHS.csv";
    sap_flow_file.value = "../data/hainich/eval/SAP_Hainich_Fagus-mean_dT30min_prog.csv";
    psi_stem_file.value  = "../data/hainich/eval/stem_water_pot.csv";
    parameters_list_file.value = "../tests/hainich/test/input/parameter_example_1.csv";
}

void Config::Export(std::string filename) {
    std::ofstream w_config;
    w_config.open(filename);
    if (location == Location::Swiss_cc)
        w_config << "location" << "=" << "Swiss_cc"<< "\n";
    else if (location == Location::Hainich)
        w_config << "location" << "=" << "Hainich"<< "\n";
    else{
        std::cout << "Invalid location specified";
        exit(99);
    }

    w_config << forcing_file.key << "=" << forcing_file.value << "\n";
    w_config << parameters_list_file.key << "=" << parameters_list_file.value << "\n";
    w_config << soilwater_file.key << "=" << soilwater_file.value << "\n";
    w_config << swiss_soil_water_input_type.key << "=" << "nlayers_mean_n_std\n";
    w_config << swiss_tree_folder_path.key << "=" << swiss_tree_folder_path.value<< "\n";
    w_config << psi_stem_file.key << "=" << psi_stem_file.value<< "\n";
    w_config << sap_flow_file.key << "=" << sap_flow_file.value<< "\n";
}

