//
// Created by Phillip on 27.07.23.
//
#pragma once
#include "../framework/simulation_multi.h"
#include "../io/analysis_swiss.h"
#include "../io/swiss_drought_trees.h"
#include "date_time.h"

class Simulation_Multi_Swiss : public Simulation_Multi{

public:
    Simulation_Multi_Swiss(int rank, bool split_paramlist);
    void Init_eval(DateTime timestart, DateTime timeend) override;
    vector<Analysis_Swiss> Get_analysis_list();
private:
    void Update_Analysis(const Model &model, const Parameters &parameters) override;
    std::unique_ptr<Swiss_Drought_Trees> swiss_trees;
    std::vector<Analysis_Swiss> analysis_list;
};



