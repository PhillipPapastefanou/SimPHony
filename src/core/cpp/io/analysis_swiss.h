//
// Created by Phillip on 14.07.23.
//
#pragma once
#include "output.h"
#include <memory>

class Model;
class TimeSlice;
class Swiss_Drought_Trees;
class Tree_Psi_Leaf_State;

class Analysis_Swiss {

public:
    Analysis_Swiss(Model* model, const Swiss_Drought_Trees& swiss_drought_trees, const Parameters& parameters);

    void Run();

    std::vector<TimeSlice> Get_time_slices();
    std::vector<double> Get_rmse();

private:

    double dts;

    const Output& output;

    float find_overall_peak(const std::vector<float>& values) ;

    void run_peak_analysis();
    void compare_psi_model_obs();

    std::vector<TimeSlice> slices;
    const Swiss_Drought_Trees& swiss_drought_trees;
    std::shared_ptr<Tree_Psi_Leaf_State> swiss_psi_leaf_states;

};

class TimeSlice{

public:

    TimeSlice();
    void Init(std::string name, const std::vector<float>& values, double ts_min, double ts_max, double dts);

    void CalculatePeaks();


    std::string name;

    double ts_min;
    double ts_max;
    double ts_max_inc;

    int day_min;
    int day_max_inc;

    int i_min;
    int i_max;

    double minimum;
    double maximum;

private:

    std::vector<float> slice;

    void find_peak();


};

class Tree_Psi_Leaf_State{

public:
    Tree_Psi_Leaf_State(const Swiss_Drought_Trees& swiss_drought_trees, const  vector<DateTime>& sim_dates);

    void Calculate_rmse(const std::vector<float>& values, const double dts);

    std::vector<double> Get_rmse_data();

private:
    const Swiss_Drought_Trees& swiss_drought_trees;

    std::vector<double> rmse_data;
    const std::vector<DateTime>& sim_dates;


};



