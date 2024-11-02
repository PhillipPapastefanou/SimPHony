//
// Created by Phillip on 14.07.23.
//
#pragma once
#include "../framework/simulation_single.h"
#include "../io/analysis_swiss.h"
#include "../io/swiss_drought_trees.h"
#include "date_time.h"

class Simulation_Single_Swiss : public  Simulation_Single{

public:
    Simulation_Single_Swiss();
    Analysis_Swiss Get_analysis();

    void Init_eval(DateTime timestart, DateTime timeend) override;
    void Analyse() override;

private:
    std::unique_ptr<Swiss_Drought_Trees> swiss_trees;
    std::unique_ptr<Analysis_Swiss> analysis;

};


