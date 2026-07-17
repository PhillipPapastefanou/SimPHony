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

    void Set_Forcing_Data(const vector<long>& timestamps, 
                          const vector<float>& vpd_in, 
                          const vector<float>& rad_in, 
                          const vector<vector<float>>& theta_in);

    void Set_Forcing_Data_Fast(uintptr_t ts_ptr, uintptr_t vpd_ptr,
                            uintptr_t rad_ptr, uintptr_t theta_ptr,
                            int n, int n_layers);

    void Set_Forcing_Data_Blob(uintptr_t blob_ptr, int n, int n_layers);

private:
    std::unique_ptr<InputCollection> forcing_parser;
    vector<float> rad;
};
