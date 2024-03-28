//
// Created by Phillip on 14.07.23.
//

#include "parameter_csv_reader.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <memory>
using std::string;

Parameter_CSV_Reader::Parameter_CSV_Reader(std::string filename): reader(filename, true, ','), check_if_found(false) {

}

void Parameter_CSV_Reader::Parse_Full_Files() {

    // Default parameters;
    Parameters params;

    parse_parameters(params, false);
}


void Parameter_CSV_Reader::Parse_Partial_Files(const Parameters& parameters) {

    Parameters params = parameters;

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

        vector<string> row = values[i];

        val = "root_area_index";
        pos = get_position(val);
        if(pos <  row.size())
            params.root_area_index = std::stod(row[pos]);

        val = "canopy_height";
        pos = get_position(val);
        if(pos <  row.size())
            params.canopy_height = std::stod(row[pos]);

//        val = "rho_water";
//        pos = get_position(val);
//        if(pos <  row.size())
//            params.rho_water = std::stod(row[pos]);
//
//        val = "grav";
//        pos = get_position(val);
//        if(pos <  row.size())
//            params.grav = std::stod(row[pos]);

        val = "eta_LS";
        pos = get_position(val);
        if(pos <  row.size())
            params.eta_LS = std::stod(row[pos]);

        val = "stem_hydraulic_capacitance";
        pos = get_position(val);
        if(pos <  row.size())
            params.stem_hydraulic_capacitance_max = std::stod(row[pos]);

        val = "k_xylem_sat";
        pos = get_position(val);
        if(pos <  row.size())
            params.k_xylem_sat = std::stod(row[pos]);

        val = "huber_value";
        pos = get_position(val);
        if(pos <  row.size())
            params.huber_value = std::stod(row[pos]);

        val = "psi50_xylem";
        pos = get_position(val);
        if(pos <  row.size())
            params.psi50_xylem = std::stod(row[pos]);

        val = "d_50_s";
        pos = get_position(val);
        if(pos <  row.size())
            params.d_50_s = std::stod(row[pos]);

        val = "leaf_hydraulic_capacitance";
        pos = get_position(val);
        if(pos <  row.size())
            params.leaf_hydraulic_capacitance = std::stod(row[pos]);

        val = "leaf_area_index";
        pos = get_position(val);
        if(pos <  row.size())
            params.leaf_area_index = std::stod(row[pos]);

        val = "psi_leaf_50_close";
        pos = get_position(val);
        if(pos <  row.size())
            params.psi_leaf_50_close = std::stod(row[pos]);

        val = "d_50_close";
        pos = get_position(val);
        if(pos <  row.size())
            params.d_50_close = std::stod(row[pos]);

        val = "g0";
        pos = get_position(val);
        if(pos <  row.size())
            params.g0 = std::stod(row[pos]);

        val = "g1";
        pos = get_position(val);
        if(pos <  row.size())
            params.g1 = std::stod(row[pos]);

        val = "soil_depths";
        pos = get_position(val);
        if(pos <  row.size()){
            params.soil_depths.resize(0);
            std::string input = row[pos];
            std::string word;
            std::stringstream row_str(input);

            while (getline(row_str, word, ';')) {
                params.soil_depths.push_back(std::stod(word));
            }
        }

        val = "jackson_root_beta";
        pos = get_position(val);
        if(pos <  row.size())
            params.jackson_root_beta = std::stod(row[pos]);

        val = "k_soil_sat";
        pos = get_position(val);
        if(pos <  row.size())
            params.k_soil_sat = std::stod(row[pos]);

        val = "theta_s";
        pos = get_position(val);
        if(pos <  row.size())
            params.theta_s = std::stod(row[pos]);

        val = "theta_r";
        pos = get_position(val);
        if(pos <  row.size())
            params.theta_r = std::stod(row[pos]);

        val = "alpha_genucht";
        pos = get_position(val);
        if(pos <  row.size())
            params.alpha_genucht = std::stod(row[pos]);

        val = "n_genucht";
        pos = get_position(val);
        if(pos <  row.size())
            params.n_genucht = std::stod(row[pos]);

        val = "neta_genucht";
        pos = get_position(val);
        if(pos <  row.size())
            params.neta_genucht = std::stod(row[pos]);

        val = "camp_b";
        pos = get_position(val);
        if(pos <  row.size())
            params.camp_b = std::stod(row[pos]);

        val = "camp_psi_soil_ref";
        pos = get_position(val);
        if(pos <  row.size())
            params.camp_psi_soil_ref = std::stod(row[pos]);

        val = "solver_precision";
        pos = get_position(val);
        if(pos <  row.size())
            params.solver_precision = std::stod(row[pos]);

        val = "theta_emp_multiplier";
        pos = get_position(val);
        if(pos <  row.size())
            params.theta_emp_multiplier = std::stod(row[pos]);

        val = "tree_density";
        pos = get_position(val);
        if(pos <  row.size())
            params.tree_density = std::stod(row[pos]);

        val = "sand_frac";
        pos = get_position(val);
        if(pos <  row.size())
            params.sand_frac = std::stod(row[pos]);

        val = "clay_frac";
        pos = get_position(val);
        if(pos <  row.size())
            params.clay_frac = std::stod(row[pos]);

        val = "organic_matter_frac";
        pos = get_position(val);
        if(pos <  row.size())
            params.organic_matter_frac = std::stod(row[pos]);

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
                std::cout<< "Invalid soil water type: .";
                std::cout << type_str;
                exit(99);
            }
        }

        if (check_all){
            if(this->n_conversion != row.size() - 1){
                std::cout << "Element parsing mismatch, some elements of the input file have not been parsed or some elements have been parsed multiple times" << std::endl;
                std::cout << "Parsed elements: " << this->n_conversion << std::endl;
                std::cout << "Elements available: " << row.size() - 1 << std::endl;
                throw;
            }
        }
        parameters_list.push_back(params);
    }

}
