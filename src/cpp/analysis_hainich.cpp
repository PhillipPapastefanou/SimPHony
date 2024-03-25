//
// Created by Phillip on 16.03.24.
//

#include "analysis_hainich.h"
#include "model.h"
#include <algorithm>
#include <iostream>

AnalysisHainich::AnalysisHainich(Leaf_Stem_Implicit_Model* model, const Parameters& parameters):
output(model->Get_output()), dts(model->dts), parameters(parameters){
}

AnalysisHainich::~AnalysisHainich() {

}



void AnalysisHainich::CompareSapwood(const TimeSeries &time_series) {

    auto J = output.Get_J();
    auto G  = output.Get_G();

    std::vector<double> diff_J;
    std::vector<double> diff_G;

    int i = 0;
    for (auto index : time_series.model_datetime_indexes){

        double J_obs = time_series.data[i][0];

        //Convert from kg to g
        J_obs *= parameters.KG_to_G;

        //Conver from g to molH2o
        J_obs *= parameters.G_H2O_To_Mol;

        // Multiply from flux m_sapwood^2 to flux m_total_area^2
        J_obs *= parameters.tree_density;

        diff_J.push_back(J_obs - J[index]);
        diff_G.push_back(J_obs - G[index]);

        i++;
    }

    rmse_G = RMSE(diff_G);
    rmse_J = RMSE(diff_J);

}

double AnalysisHainich::RMSE(const std::vector<double>& diffs) {
    double rmse = 0.0;
    for (double d : diffs){
        rmse += (d * d);
    }
    rmse /= diffs.size();
    return std::sqrt(rmse);
}

double AnalysisHainich::Get_Rmse_G() {
    return rmse_G;
}

double AnalysisHainich::Get_Rmse_J() {
    return rmse_J;
}
