//
// Created by Phillip on 14.07.23.
//

#include "simulation_single_swiss.h"
#include "../io/input_swiss_mult_soils.h"

Simulation_Single_Swiss::Simulation_Single_Swiss(){
}

void Simulation_Single_Swiss::Init_eval(DateTime timestart, DateTime timeend) {
    swiss_trees = std::make_unique<Swiss_Drought_Trees>(config->swiss_tree_folder_path.value);
}

void Simulation_Single_Swiss::Analyse() {
    analysis = std::make_unique<Analysis_Swiss>(*model, *swiss_trees, *parameters);
    analysis->Run();
}

Analysis_Swiss Simulation_Single_Swiss::Get_analysis() {
    return *analysis;
}
