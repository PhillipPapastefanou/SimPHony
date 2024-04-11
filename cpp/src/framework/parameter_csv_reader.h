//
// Created by Phillip on 14.07.23.
//

#include "parameters.h"
#include "../auxil/csv_reader.h"
#include <string>
#pragma once


class Parameter_CSV_Reader {

public:
    Parameter_CSV_Reader(std::string filename);

    void Parse_Full_Files();
    void Parse_Partial_Files(const Parameters& parameters);

    const vector<Parameters> &Get_parameter_list() const;


private:
    io::CSV_Reader reader;

    bool check_if_found;

    int get_position(std::string value);
    std::vector<Parameters> parameters_list;

    int n_conversion;

    void parse_parameters(Parameters parameters, bool check_all);

};

