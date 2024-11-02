//
// Created by Phillip on 31/10/2024.
//
#pragma once
#include "parameters.h"
#include "date_time.h"
#include "../io/input.h"
#include "../io/output.h"
#include "../modules/model.h"
#include "../framework/config.h"
#include <memory>

class Simulation_Single {

public:
    Simulation_Single();
    void Read_Config(std::string config_path);
    void Init_input();
    virtual void Init_eval(DateTime timestart, DateTime timeend);
    void Init_parameters_default();
    void Init_parameters_filename(int index);
    void Init_parameters(Parameters params);
    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(DateTime timestart, DateTime timeend);

    Output Get_output();

protected:
    double psi_leaf_init;
    double psi_stem_init;

    std::unique_ptr<Config> config;
    std::unique_ptr<Parameters> parameters;
    std::unique_ptr<Input> input;
    std::unique_ptr<Model> model;

    virtual void Analyse();
};