//
// Created by Phillip on 16.03.24.
//
#pragma once
#include "../io/output.h"
#include "../io/input_hainich.h"
#include "../io/input.h"
#include "../modules/model.h"
#include "parameters.h"
#include <memory>
#include "../io/analysis_hainich.h"
#include "date_time.h"
#include "../io/time_series.h"

class Simulation_Single_Hainich {

public:
    Simulation_Single_Hainich();

    void Init_input(std::string forcing_file,
                    std::string sapflux_file,
                    std::string psi_stem_file);

    void Init_parameters_default();
    void Init_parameters_filename(string filename, int index);

    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(DateTime timestart, DateTime timeend);

    Output Get_output();
    AnalysisHainich Get_analysis();
private:
    std::string theta_file;
    std::string forcing_file;

    std::unique_ptr<Parameters> parameters;
    std::unique_ptr<Input_Hainich> input;

    std::unique_ptr<TimeSeries> sap_series;
    std::unique_ptr<TimeSeries> psi_stem_series;

    std::unique_ptr<Model> model;
    std::unique_ptr<AnalysisHainich> analysis;
};

