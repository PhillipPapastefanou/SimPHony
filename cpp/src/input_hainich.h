//
// Created by Phillip on 11.07.23.
//

#pragma once
#include "input_collection.h"
#include "parameters.h"
#include "date_time.h"
#include <string>
#include <vector>
#include "input.h"


using std::string;
using std::vector;
using std::unique_ptr;

class Input_Hainich: public Input{

public:
    Input_Hainich(const Parameters& parameters);

    void Read_N_Parse() override;

    void Add_Forcing_File(std::string file) override;
    void Add_Soilwater_File(std::string file) override;

private:
    std::unique_ptr<InputCollection> forcing_parser;
    vector<float> rad;
    string forcing_file;

};
