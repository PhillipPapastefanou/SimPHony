//
// Created by Phillip on 16.03.24.
//
#pragma once
#include "../framework/simulation_single.h"
#include "../modules/model.h"
#include "../io/analysis_hainich.h"
#include "date_time.h"


class Simulation_Single_Hainich: public Simulation_Single{

public:
    Simulation_Single_Hainich();
    AnalysisHainich Get_analysis();

    void Init_eval(DateTime timestart, DateTime timeend) override;
    void Analyse() override;

private:
    std::unique_ptr<TimeSeries> sap_series;
    std::unique_ptr<TimeSeries> psi_stem_series;
    std::unique_ptr<AnalysisHainich> analysis;
    
};

