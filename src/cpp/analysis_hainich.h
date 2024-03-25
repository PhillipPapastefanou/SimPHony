//
// Created by Phillip on 16.03.24.
//
#pragma once
#include "output.h"
#include <memory>
#include "time_series.h"
#include "parameters.h"

class Leaf_Stem_Implicit_Model;
class AnalysisHainich {


public:
    AnalysisHainich(Leaf_Stem_Implicit_Model* model, const Parameters& parameters);

    void CompareSapwood(const TimeSeries& time_series);

    ~AnalysisHainich();

    double Get_Rmse_J();
    double Get_Rmse_G();

private:
    const Output& output;
    const Parameters& parameters;
    double dts;

    double RMSE(const std::vector<double>& diffs);

    double rmse_J;
    double rmse_G;

};