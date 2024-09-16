//
// Created by Phillip on 16.03.24.
//

#include "analysis_hainich.h"
#include "../modules/model.h"
#include <algorithm>
#include <iostream>

AnalysisHainich::AnalysisHainich(Model* model, const Parameters& parameters):
output(model->Get_output()), parameters(parameters),
sigma_log_likelyhood(parameters.sigma_log_likelyhood){
}

AnalysisHainich::~AnalysisHainich() {

}



void AnalysisHainich::CompareSapwood(const TimeSeries &time_series) {

    const std::vector<float> J_raw = output.Get_J();
    const std::vector<float> G_raw = output.Get_G();

    std::vector<double> J_mod_arr;
    std::vector<double> G_mod_arr;
    std::vector<double> J_obs_arr;

    int i = 0;
    for (auto index : time_series.model_datetime_indexes) {
        double J_obs = time_series.data[i][0];

        J_mod_arr.push_back(J_raw[index]);
        G_mod_arr.push_back(G_raw[index]);

        //Convert from kg to g
        J_obs *= parameters.constants.KG_to_G;
        //Conver from g to molH2o
        J_obs *= parameters.constants.G_H2O_To_Mol;
        // Multiply from flux m_sapwood^2 to flux m_total_area^2
        J_obs *= parameters.tree_density;
        J_obs_arr.push_back(J_obs);
        i++;
    }

    rmse_G = calc_RMSE(G_mod_arr, J_obs_arr);
    rmse_J = calc_RMSE(J_mod_arr, J_obs_arr);

    log_likelyhood_G = calc_log_likelyhood(J_obs_arr, G_mod_arr);
    log_likelyhood_J = calc_log_likelyhood(J_obs_arr, J_mod_arr);
}

double AnalysisHainich::calc_RMSE(const std::vector<double>& d1, const std::vector<double>& d2) {

    if (d1.size() != d2.size()){
        std::cout << "Different sizes in vectors at the RMSE calculation ";
        exit(99);
    }

    double rmse = 0.0;
    for (int i = 0; i < d1.size(); ++i) {
        rmse += (d1[i] - d2[i]) * (d1[i] - d2[i]);
    }
    rmse /= d1.size();
    return std::sqrt(rmse);
}

double AnalysisHainich::calc_log_likelyhood(const vector<double> &obs, const vector<double> &mod) {

    if (obs.size() != mod.size()){
        std::cout << "Different sizes in vectors at the RMSE calculation ";
        exit(99);
    }
    // Log likelyhood value
    double llv = 0.0;
    for (int i = 0; i < obs.size(); ++i) {

        double obs_value = obs[i];
        double mod_value = mod[i];

        // x = observation
        // mu = model value
        double normal = normal_pdf(mod_value,obs_value);
        llv += std::log(normal);
    }
    return llv;
}


double AnalysisHainich::Get_Rmse_G() {
    return rmse_G;
}

double AnalysisHainich::Get_Rmse_J() {
    return rmse_J;
}

double AnalysisHainich::Get_Log_Likelyhood_J() {
    return log_likelyhood_J;
}

double AnalysisHainich::Get_Log_Likelyhood_G() {
    return log_likelyhood_G;
}

double AnalysisHainich::normal_pdf(double mu, double x) {
    const double sigma = sigma_log_likelyhood;
    double alpha = 1.0 / (std::sqrt(2.0 * parameters.constants.PI) * sigma);
    return alpha *  std::exp(-(x-mu)*(x-mu)/(2*sigma*sigma));
}

void AnalysisHainich::ComparePsiStem(const TimeSeries &time_series) {

    const std::vector<float> psi_stem_raw = output.Get_psi_stem();

    std::vector<double> psi_stem_mod_arr;
    std::vector<double> psi_stem_obs_arr;

    int i = 0;
    for (auto index : time_series.model_datetime_indexes) {
        double psi_stem_obs = time_series.data[i][0];
        psi_stem_mod_arr.push_back(psi_stem_raw[index]);
        psi_stem_obs_arr.push_back(psi_stem_obs);
        i++;
    }

    rmse_psi_stem = calc_RMSE(psi_stem_mod_arr, psi_stem_obs_arr);

    log_likelyhood_psi_stem = calc_log_likelyhood(psi_stem_obs_arr, psi_stem_mod_arr);

}
double AnalysisHainich::Get_Rmse_psi_stem() {
    return rmse_psi_stem;
}

double AnalysisHainich::Get_Log_Likelyhood_psi_stem() {
    return log_likelyhood_psi_stem;
}



