//
// Created by Phillip on 14.07.23.
//

#include "parameter_csv_reader.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <memory>
#include <cstring>
using std::string;

Parameter_CSV_Reader::Parameter_CSV_Reader(std::string filename): reader(filename, true, ','), check_if_found(false) {

}

void Parameter_CSV_Reader::Parse_Full_Files() {

    // Default parameters;
    Parameters params;

    check_if_found = true;

    parse_parameters(params, false);
}


void Parameter_CSV_Reader::Parse_Partial_Files(const Parameters& parameters) {

    Parameters params = parameters;

    check_if_found = false ;

    parse_parameters(params, false);

}


int Parameter_CSV_Reader::get_position(std::string value) {
    auto pos = std::distance(reader.header.begin(), std::find(reader.header.begin(), reader.header.end(), value));

    if(pos == reader.header.size() && check_if_found){
        std::cout<< "Could not find element: " << value <<std::endl;
        throw;
    }
    // Add up how many parameters have been read to ensure that we have read the same amount of parameters that also exist in the file
    if (pos < reader.header.size())
        n_conversion ++;
    return pos;
}

const vector<Parameters> &Parameter_CSV_Reader::Get_parameter_list() const {
    return parameters_list;
}

void Parameter_CSV_Reader::parse_parameters(Parameters parameters, bool check_all) {

    vector<vector<string>> values = reader.Get<string>();
    size_t nfiles = values.size();
    //Parameter conversions
    for (int i = 0; i < nfiles; ++i) {

        Parameters params = parameters;
        string val;
        int pos;
        n_conversion = 0;

        row = values[i];

        parse_value(params.id, "id");
        parse_value(params.verbose, "verbose");
        parse_value(params.max_psi_leaf_change_per_hour, "max_psi_leaf_change_per_hour");
        parse_value(params.minimum_psi_leaf_multiplier, "minimum_psi_leaf_multiplier");
        parse_value(params.root_area_index, "root_area_index");
        parse_value(params.canopy_height, "canopy_height");
        parse_value(params.eta_LS, "eta_LS");
        parse_value(params.stem_hydraulic_capacitance_max, "stem_hydraulic_capacitance");
        parse_value(params.leaf_hydraulic_capacitance, "leaf_hydraulic_capacitance");
        parse_value(params.k_xylem_sat, "k_xylem_sat");
        parse_value(params.huber_value, "huber_value");
        parse_value(params.psi50_xylem, "psi50_xylem");
        parse_value(params.psi88_xylem, "psi88_xylem");

        parse_value(params.leaf_area_index,  "leaf_area_index");
        parse_value(params.psi_leaf_50_close,  "psi_leaf_50_close");
        parse_value(params.d_50_close,  "d_50_close");
        parse_value(params.g0,  "g0");
        parse_value(params.g1,  "g1");
        parse_value(params.anet_max,  "anet_max");
        parse_value(params.sw_rad_max, "sw_rad_max");
        parse_value(params.g_bark, "g_bark");
    

        val = "soil_depths";
        pos = get_position(val);
        if(pos <  row.size()){

            std::vector<double> soil_depths;
            soil_depths.resize(0);
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            while (getline(row_str, word, ';')) {
                soil_depths.push_back(std::stod(word));
            }

            params.soil_layers.resize(soil_depths.size());
            for (int j = 0; j < params.soil_layers.size(); ++j) {
                params.soil_layers[j].depth = soil_depths[j];
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }


        val = "k_soil_sats";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].k_soil_sat = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }

        parse_value(params.jackson_root_beta,  "jackson_root_beta");

        val = "theta_s";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].theta_s = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }

        val = "theta_r";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].theta_r = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }

        val = "pore_size_ind";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].pore_size_ind = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }

        val = "camp_b";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].camp_b = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }

        val = "psi_soil_sats";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].psi_soil_sat = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }


        parse_value(params.solver_precision,  "solver_precision");
        parse_value(params.wcont_sigma_deviation, "wcont_sigma_deviation");
        parse_value(params.tree_density,  "tree_density");


        val = "sand_fracs";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].sand_fraction = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }


        val = "clay_fracs";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].clay_fraction = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }

        val = "organic_matter_fracs";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);
            int i = 0;
            while (getline(row_str, word, ';')) {
                params.soil_layers[i].organic_matter_fraction = std::stod(word);
                i++;
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }


        parse_value(params.soil_profile_index,  "soil_profile_index");
        parse_value(params.sigma_log_likelyhood,  "sigma_log_likelyhood");

        val = "soil_water_model_type";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string type_str = row[pos];
            if(type_str == "Saxton06")
                params.soil_water_type = Soil_water_module_type::Saxton06;
            else if(type_str == "Campbell")
                params.soil_water_type = Soil_water_module_type::Campbell;
            else if(type_str == "VanGenuchten")
                params.soil_water_type = Soil_water_module_type::VanGenuchten;
            else{
                std::cout<< "Invalid soil water type:";
                std::cout << type_str;
                exit(99);
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }

        val = "stem_flow_type";
        pos = get_position(val);
        if(pos <  row.size()){
            std::string type_str = row[pos];
            if(type_str == "Linear")
                params.stem_flow_type = Stem_flow_module_type::Linear;
            else if(type_str == "KirchhoffWeibull")
                params.stem_flow_type = Stem_flow_module_type::KirchhoffWeibull;
            else if(type_str == "KirchhoffPiecewiseErf")
                params.stem_flow_type = Stem_flow_module_type::KirchhoffPiecewiseErf;
            else{
                std::cout<< "Invalid stem flow type:";
                std::cout << type_str;
                exit(99);
            }
        }
        else{
            if (check_if_found){
                std::cout << "could not parse " << val << std::endl;
                std::cout << "Exiting";
                exit(99);
            }
        }
        
        parameters_list.push_back(params);
    }

}

void Parameter_CSV_Reader::parse_value(double &param, std::string value ) {

    int pos = get_position(value);
    if(pos < row.size())
        param = std::stod(row[pos]);
    else{
        if (check_if_found){
            std::cout << "could not parse " << value << std::endl;
            std::cout << "Exiting";
            exit(99);
        }
    }
}

void Parameter_CSV_Reader::parse_value(int &param, std::string value) {

    int pos = get_position(value);
    if(pos < row.size())
        param = std::stoi(row[pos]);
    else{
        if (check_if_found){
            std::cout << "could not parse " << value << std::endl;
            std::cout << "Exiting";
            exit(99);
        }
    }

}

void Parameter_CSV_Reader::parse_value(bool &param, std::string value) {

    int pos = get_position(value);
    if(pos < row.size())
        if(strcasecmp("true", row[pos].c_str()) == 0)
            param = true;
        else
            param = false;
    else{
        if (check_if_found){
            std::cout << "could not parse " << value << std::endl;
            std::cout << "Exiting";
            exit(99);
        }
    }
}

