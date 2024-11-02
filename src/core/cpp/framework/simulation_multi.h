//
// Created by Phillip on 01/11/2024.
//

# pragma once
#include <string>
#include "parameters.h"
#include "date_time.h"
#include "../io/input.h"
#include "../io/output.h"
#include "../modules/model.h"
#include "../framework/config.h"

class Simulation_Multi {

public:
    Simulation_Multi(int rank, bool split_paramlist);
    void Read_Config(std::string config_path);
    void Init_input();
    void Init_Full_Parameter_Setups(std::vector<int> ids);
    void Init_Partial_Parameter_Setups(std::vector<int> ids);
    void Set_water_pot_initials(double psi_leaf, double psi_stem);
    void Run(DateTime timestart, DateTime timeend);
    virtual void Init_eval(DateTime timestart, DateTime timeend);

protected:
    int rank;
    bool split_paramlist = false;

    std::unique_ptr<Config> config;
    std::vector<std::tuple<Parameters, int> > parameter_list;
    std::unique_ptr<Input> input;

    double init_psi_stem;
    double init_psi_leaf;

    virtual void Update_Analysis(const Model& model,  const Parameters& parameters);

};
