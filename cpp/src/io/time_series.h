//
// Created by Phillip on 16.03.24.
//

#pragma once
#include "../framework/date_time.h"
#include <string>
#include <vector>
#include "input_collection.h"

using std::string;

class TimeSeries {
public:
    TimeSeries(string filename, bool has_header, char delimiter);

    void Load(string dt_header, string format, std::vector<int> data_index);
    void GenerateModelObsIndexes(DateTime begin, DateTime end, long timestep);

    vector<vector<float>> data;
    vector<int> model_datetime_indexes;
    virtual ~TimeSeries();
private:
    InputCollection input;

};

