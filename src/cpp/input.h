//
// Created by Phillip on 11.07.23.
//

#pragma once
#include "csv_reader.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class Input {


public:
    Input(string theta_file, string forcing_reader);

    void Read_N_Parse();

    // According to the forcing input [W m-2]
    const double SWDOWN_MAX = 1040;

    // Maximum net photosythesis rate [xxx]
    // According to the excel sheet of Arend 2021 appendix
    const double ANET_MAX = 5.7;

    ///
    vector<float> anet;
    /// Vapour pressure deficit [Pa]
    vector<float> vpd;
    /// Volumetric water content [m3 m-3]
    vector<vector<float> > theta_per_layer;

private:
    io::CSV_Reader theta_per_layer_reader;
    io::CSV_Reader forcing_reader;

    vector<float> rad;


};
