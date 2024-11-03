//
// Created by Phillip on 16.03.24.
//

#include "simulation_multi_hainich.h"
#include "parameters.h"
#include "parameter_csv_reader.h"
#include "../modules/model.h"
#include "../auxil/misc.h"
#include <iostream>


Simulation_Multi_Hainich::Simulation_Multi_Hainich(int rank, bool split_paramlist): Simulation_Multi(rank, split_paramlist)  {
}



void Simulation_Multi_Hainich::Init_eval(DateTime timestart, DateTime timeend) {
    sap_series = std::make_unique<TimeSeries>(config->sap_flow_file.value, true, ',');
    sap_series->Load("datetime", "%Y-%m-%d %H:%M:%S", {1});

    psi_stem_series = std::make_unique<TimeSeries>(config->psi_stem_file.value, true, ',');
    psi_stem_series->Load("time", "%Y-%m-%d %H:%M:%S", {1});

    double dts = std::get<0>(parameter_list[0]).dts;
    sap_series->GenerateModelObsIndexes(timestart, timeend, dts);
    psi_stem_series->GenerateModelObsIndexes(timestart, timeend, dts);
}

void Simulation_Multi_Hainich::Update_Analysis(const Model &model, const Parameters &parameters) {

    AnalysisHainich analysis(model, parameters);
    analysis.CompareSapwood(*sap_series);
    analysis.ComparePsiStem(*psi_stem_series);
    analysis_list.push_back(analysis);
}

std::vector<AnalysisHainich> Simulation_Multi_Hainich::Get_analysis_list() {
    return analysis_list;
}
