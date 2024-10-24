//
// Created by Phillip on 16.03.24.
//

#pragma once
#include "../io/output.h"
#include "../io/input_hainich.h"
#include "parameters.h"
#include <memory>
#include "../io/analysis_hainich.h"
#include <tuple>
#include <chrono>
#include "date_time.h"

class Simulation_Multi_Hainich {

public:
    Simulation_Multi_Hainich();
    void Init_input(std::string forcing_file, std::string sap_flow_file, std::string psi_stem_file, int rank);
    void Init_Full_Parameter_Setups(string filename, std::vector<int> ids);
    void Init_Partial_Parameter_Setups(string root_filename, string partial_parameter_filename, std::vector<int> ids);

    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(DateTime timestart, DateTime timeend);

    std::vector<AnalysisHainich> Get_analysis_list();

    DateTime Get_first_year();
    DateTime Get_last_year();
private:

    int rank;

    std::unique_ptr<Input_Hainich> input;
    std::vector<std::tuple<Parameters, int> > parameter_list;

    std::unique_ptr<TimeSeries> sap_series;
    std::unique_ptr<TimeSeries> psi_stem_series;

    std::vector<AnalysisHainich> analysis_list;

    double init_psi_stem;
    double init_psi_leaf;
};

