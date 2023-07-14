//
// Created by Phillip on 14.07.23.
//

#include "parameters.h"
#include "csv_reader.h"
#include <string>
#pragma once


class Parameter_CSV_Reader {

public:
    Parameter_CSV_Reader(std::string filename);

    void Parse();

private:
    CSV_Reader reader;

    int get_position(std::string value);
    std::vector<Parameters> parameters_list;

    int n_conversion;

};

