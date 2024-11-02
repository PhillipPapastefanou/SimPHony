//
// Created by Phillip on 11.07.23.
//

#pragma once
#include "input_collection.h"
#include "../framework/parameters.h"
#include "../framework/date_time.h"
#include <string>
#include <vector>
#include "input.h"


using std::string;
using std::vector;
using std::unique_ptr;

class Input_Swiss_Multi_Soils: public Input{

public:
    Input_Swiss_Multi_Soils(const Config& config);
    void Read_N_Parse() override;

private:
    vector<vector<float>> all_theta_data;
    std::unique_ptr<InputCollection> theta_parser;
    std::unique_ptr<InputCollection> forcing_parser;
};
