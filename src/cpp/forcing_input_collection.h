//
// Created by Phillip on 11.03.24.
//
#pragma once
#include "csv_reader.h"
#include <string>
#include <vector>
#include "date_time.h"

class ForcingInputCollection {

public:
    ForcingInputCollection(std::string filename, bool has_header, char delimiter);

    void init(std::string dt_header, std::string format);

    vector<vector<float> > get_data(vector<int> indexes);

    std::vector<DateTime> dates;

private:
    io::CSV_Reader reader;

};


