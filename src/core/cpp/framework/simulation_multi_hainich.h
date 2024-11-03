//
// Created by Phillip on 16.03.24.
//

#pragma once
#include "../framework/simulation_multi.h"
#include "../io/analysis_hainich.h"
#include "../io/swiss_drought_trees.h"
#include "date_time.h"


class Simulation_Multi_Hainich: public Simulation_Multi{

public:
    Simulation_Multi_Hainich();
    Simulation_Multi_Hainich(int rank, bool split_paramlist);
    void Init_eval(DateTime timestart, DateTime timeend) override;
    vector<AnalysisHainich> Get_analysis_list();
private:
    void Update_Analysis(const Model &model, const Parameters &parameters) override;
    std::unique_ptr<TimeSeries> sap_series;
    std::unique_ptr<TimeSeries> psi_stem_series;
    std::vector<AnalysisHainich> analysis_list;

};

