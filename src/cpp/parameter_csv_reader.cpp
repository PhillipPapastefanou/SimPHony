//
// Created by Phillip on 14.07.23.
//

#include "parameter_csv_reader.h"
#include <iostream>
using std::string;

Parameter_CSV_Reader::Parameter_CSV_Reader(std::string filename): reader(filename, true, ',') {

}

void Parameter_CSV_Reader::Parse() {


    vector<vector<string>> values = reader.Get<string>();
    size_t nfiles = values.size();


    //Parameter conversions

    for (int i = 0; i < nfiles; ++i) {

        Parameters params;
        string val;
        int pos;
        n_conversion = 0;

        vector<string> row = values[i];

        val = "root_zone_depth";
        pos = get_position(val);
        params.root_zone_depth = std::stod(row[pos]);

        val = "root_area_index";
        pos = get_position(val);
        params.root_area_index = std::stod(row[pos]);

        val = "canopy_height";
        pos = get_position(val);
        params.canopy_height = std::stod(row[pos]);

        val = "rho_water";
        pos = get_position(val);
        params.rho_water = std::stod(row[pos]);

        val = "grav";
        pos = get_position(val);
        params.grav = std::stod(row[pos]);

        val = "eta_LS";
        pos = get_position(val);
        params.eta_LS = std::stod(row[pos]);

        val = "stem_hydraulic_capacitance";
        pos = get_position(val);
        params.stem_hydraulic_capacitance = std::stod(row[pos]);

        val = "k_xylem_sat";
        pos = get_position(val);
        params.k_xylem_sat = std::stod(row[pos]);

        val = "huber_value";
        pos = get_position(val);
        params.huber_value = std::stod(row[pos]);

        val = "psi50_xylem";
        pos = get_position(val);
        params.psi50_xylem = std::stod(row[pos]);

        val = "d_50_s";
        pos = get_position(val);
        params.d_50_s = std::stod(row[pos]);

        val = "leaf_hydraulic_capacitance";
        pos = get_position(val);
        params.leaf_hydraulic_capacitance = std::stod(row[pos]);

        val = "leaf_area_index";
        pos = get_position(val);
        params.leaf_area_index = std::stod(row[pos]);

        val = "psi_leaf_50_close";
        pos = get_position(val);
        params.psi_leaf_50_close = std::stod(row[pos]);

        val = "d_50_close";
        pos = get_position(val);
        params.d_50_close = std::stod(row[pos]);

        val = "g0";
        pos = get_position(val);
        params.g0 = std::stod(row[pos]);

        val = "g1";
        pos = get_position(val);
        params.g1 = std::stod(row[pos]);

        val = "nsoil";
        pos = get_position(val);
        params.nsoil = std::stoi(row[pos]);

        val = "layer_depth";
        pos = get_position(val);
        params.layer_depth = std::stod(row[pos]);

        val = "min_soil_layer_depth";
        pos = get_position(val);
        params.min_soil_layer_depth = std::stod(row[pos]);

        val = "max_soil_layer_depth";
        pos = get_position(val);
        params.max_soil_layer_depth = std::stod(row[pos]);

        val = "jackson_root_beta";
        pos = get_position(val);
        params.jackson_root_beta = std::stod(row[pos]);

        val = "k_soil_sat";
        pos = get_position(val);
        params.k_soil_sat = std::stod(row[pos]);

        val = "theta_s";
        pos = get_position(val);
        params.theta_s = std::stod(row[pos]);

        val = "theta_r";
        pos = get_position(val);
        params.theta_r = std::stod(row[pos]);

        val = "alpha_genucht";
        pos = get_position(val);
        params.alpha_genucht = std::stod(row[pos]);

        val = "n_genucht";
        pos = get_position(val);
        params.n_genucht = std::stod(row[pos]);

        val = "neta_genucht";
        pos = get_position(val);
        params.neta_genucht = std::stod(row[pos]);

        val = "camp_b";
        pos = get_position(val);
        params.camp_b = std::stod(row[pos]);

        val = "camp_psi_soil_ref";
        pos = get_position(val);
        params.camp_psi_soil_ref = std::stod(row[pos]);

        val = "solver_precision";
        pos = get_position(val);
        params.solver_precision = std::stod(row[pos]);


        if(this->n_conversion != row.size() - 1){
            std::cout << "Element parsing mismatch, some elements of the input file have not been parsed or some elements have been parsed multiple times" << std::endl;
            throw;
        }
        parameters_list.push_back(params);
    }



    //parameters.layer_depth =

    double s = 1.0;

}

int Parameter_CSV_Reader::get_position(std::string value) {
    auto pos = std::distance(reader.header.begin(), std::find(reader.header.begin(), reader.header.end(), value));

    if(pos == reader.header.size()){
        std::cout<< "Cound not find element: " << value <<std::endl;
        throw;
    }
    // Add up how many parameters have been read to ensure that we have read the same amount of parameters that also exist in the file
    n_conversion ++;
    return pos;
}

const vector<Parameters> &Parameter_CSV_Reader::Get_parameter_list() const {
    return parameters_list;
}
