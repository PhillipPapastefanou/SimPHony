//
// Created by Phillip on 27.07.23.
//
#include <iostream>
#include <chrono>
#include "simulation_multi_swiss.h"
#include "parameter_csv_reader.h"
#include "../modules/model.h"
#include "../auxil/misc.h"


Simulation_Multi_Swiss::Simulation_Multi_Swiss(int rank, bool split_paramlist): Simulation_Multi(rank, split_paramlist)  {

}

void Simulation_Multi_Swiss::Init_eval(DateTime timestart, DateTime timeend) {
    swiss_trees = std::make_unique<Swiss_Drought_Trees>(config->swiss_tree_folder_path.Get());
}

void Simulation_Multi_Swiss::Update_Analysis(const Model &model, const Parameters &parameters) {

    Analysis_Swiss analysis(model, *swiss_trees, parameters);
    analysis.Run();
    analysis_list.push_back(analysis);
}

std::vector<Analysis_Swiss> Simulation_Multi_Swiss::Get_analysis_list() {
    return analysis_list;
}
