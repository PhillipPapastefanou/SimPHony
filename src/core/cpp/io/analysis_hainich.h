//
// Created by Phillip on 16.03.24.
//
#pragma once
#include "output.h"
#include <memory>
#include "time_series.h"
#include "../framework/parameters.h"

class Model;
class AnalysisHainich {


public:
    AnalysisHainich(Model* model, const Parameters& parameters);
    ~AnalysisHainich();

    void CompareSapwood(const TimeSeries& time_series);
    void ComparePsiStem(const TimeSeries& time_series);


    double Get_Rmse_J();
    double Get_Rmse_G();
    double Get_Log_Likelyhood_J();
    double Get_Log_Likelyhood_G();

    double Get_Rmse_psi_stem();
    double Get_Log_Likelyhood_psi_stem();

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

    double rmse_psi_stem;
    double log_likelyhood_psi_stem;


    double normal_pdf(double mu, double x);

};