//
// Created by Phillip on 11.03.24.
//
#pragma once
#include "../auxil/csv_reader.h"
#include <string>
#include <vector>
#include "../framework/date_time.h"

class InputCollection {

public:
    InputCollection(std::string filename, bool has_header, char delimiter);

    void init_regular(std::string dt_header, std::string format);

    void init_irregular(std::string dt_header, std::string format);

    vector<vector<float> > get_data(vector<int> indexes);

    vector<vector<float> > get_data(vector<string> columns);

    // Time resolution in seconds of input data
    long time_res_sec;
    std::vector<DateTime> dates;

private:
    io::CSV_Reader reader;

};


