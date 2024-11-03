//
// Created by Phillip on 16.03.24.
//
#include "simulation_single_hainich.h"
#include "parameter_csv_reader.h"
#include <iostream>

Simulation_Single_Hainich::Simulation_Single_Hainich(){
}

void Simulation_Single_Hainich::Init_eval(DateTime timestart, DateTime timeend) {
    sap_series = std::make_unique<TimeSeries>(config->sap_flow_file.value, true, ',');
    sap_series->Load("datetime", "%Y-%m-%d %H:%M:%S", {1});

    psi_stem_series = std::make_unique<TimeSeries>(config->psi_stem_file.value, true, ',');
    psi_stem_series->Load("time", "%Y-%m-%d %H:%M:%S", {1});

    sap_series->GenerateModelObsIndexes(timestart, timeend, parameters->dts);
    psi_stem_series->GenerateModelObsIndexes(timestart, timeend, parameters->dts);
}

void Simulation_Single_Hainich::Analyse() {
    analysis = std::make_unique<AnalysisHainich>(*model, *parameters);
    analysis->CompareSapwood(*sap_series);
    analysis->ComparePsiStem(*psi_stem_series);
}

AnalysisHainich Simulation_Single_Hainich::Get_analysis() {
    return *analysis;
}



