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

class Input_Hainich: public Input{

public:
    Input_Hainich(const Config& config);
    void Read_N_Parse() override;


private:
    std::unique_ptr<InputCollection> forcing_parser;
    vector<float> rad;
};
