//
// Created by Phillip on 16.03.24.
//

#include "time_series.h"
#include <vector>
#include <iostream>
TimeSeries::TimeSeries(std::string filename, bool has_header, char delimiter):
        input(filename, has_header, delimiter){

}

TimeSeries::~TimeSeries() {

}

void TimeSeries::Load(string dt_header, string format, std::vector<int> data_index) {

    input.init_irregular(dt_header, format);
    data = input.get_data(data_index);
}

void TimeSeries::GenerateModelObsIndexesSameRes(DateTime begin, DateTime end, long timestep) {
    int i = 0;
    std::vector<std::vector<float>> slice;
    for (DateTime dt: input.dates) {

        long s_diff = dt - begin;
        int index = s_diff / timestep;

        DateTime dr = begin.AddSeconds(s_diff);
        if((dr < end) & (dr > begin)){
            slice.push_back(data[i]);
            model_datetime_indexes.push_back(index);
        }
        i++;
    }
    data = slice;
}

void TimeSeries::GenerateModelObsIndexes(DateTime begin, DateTime end, long timestep) {

    int i = 0;
    DateTime running_dt_index = begin;
    std::vector<std::vector<float>> slice;
    // Clear the stored datetimes
    model_datetime_indexes.resize(0);

    for (DateTime dt: input.dates) {

        while (running_dt_index < end){

            if (running_dt_index == dt){

                long s_diff = dt - begin;
                int index = s_diff / timestep;
                slice.push_back(data[i]);
                model_datetime_indexes.push_back(index);
                i++;
                break;
            }

            running_dt_index = running_dt_index.AddSeconds(timestep);
        }
    }
    data = slice;
}


