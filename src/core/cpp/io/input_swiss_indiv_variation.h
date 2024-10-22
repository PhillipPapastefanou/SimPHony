//
// Created by Phillip on 08/10/2024.
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

class Input_Swiss_Indiv_Variation: public Input{

public:
    Input_Swiss_Indiv_Variation();

    void Read_N_Parse() override;
    void Add_Forcing_File(std::string file) override;
    void Add_Soilwater_File(std::string file) override;

private:
    std::unique_ptr<InputCollection> forcing_parser;
    std::unique_ptr<InputCollection> theta_parser;
    string forcing_file;
    string theta_file;

};
