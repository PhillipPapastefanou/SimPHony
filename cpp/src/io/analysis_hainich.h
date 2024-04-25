//
// Created by Phillip on 16.03.24.
//
#pragma once
#include "output.h"
#include <memory>
#include "time_series.h"
#include "../framework/parameters.h"

class Leaf_Stem_Ground_Implicit_Model;
class AnalysisHainich {


public:
    AnalysisHainich(Leaf_Stem_Ground_Implicit_Model* model, const Parameters& parameters);

    void CompareSapwood(const TimeSeries& time_series);

    ~AnalysisHainich();

    double Get_Rmse_J();
    double Get_Rmse_G();

    double Get_Log_Likelyhood_J();
    double Get_Log_Likelyhood_G();

private:
    const Output& output;
    const Parameters& parameters;
    double dts;
    double sigma_log_likelyhood;


    double calc_RMSE(const std::vector<double>& d1, const std::vector<double>& d2);
    double calc_log_likelyhood(const std::vector<double>& obs, const std::vector<double>& mod);

    double rmse_J;
    double rmse_G;

    double log_likelyhood_J;
    double log_likelyhood_G;

    double normal_pdf(double mu, double x);

};