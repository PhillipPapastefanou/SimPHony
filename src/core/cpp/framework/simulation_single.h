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
    void Init_input_web();          // like Init_input(), but skips Read_N_Parse() — forcing comes from Set_Forcing_Data_Blob() instead
    void Init_config_web_hainich();
    virtual void Init_eval(DateTime timestart, DateTime timeend);
    void Init_parameters_default();
    void Init_parameters_filename(int index);
    void Init_soil_layers_default_hainich();
    void Init_parameters(Parameters params);
    void Set_water_pot_initials(double psi_leaf, double psi_stem);

    // Sets k_soil_sat uniformly across every configured soil layer, given
    // as log10(k_soil_sat [m s-1]) rather than the raw value — saturated
    // hydraulic conductivity spans several orders of magnitude physically
    // (~1e-9 for clay to ~1e-3 for sand/gravel), so a linear slider is
    // useless for exploring it; a log10 slider covers the physical range
    // in a manageable control width. Mutates the live `parameters` object
    // directly (soil_layers isn't embind-exposed as a settable property),
    // so call this AFTER Init_parameters(), not on a Get_parameters()
    // snapshot.
    void Set_soil_k_sat_log10(double log10_k_soil_sat);
    
    void Run(DateTime timestart, DateTime timeend);
    
    void Init_eval_epoch(long start_epoch, long end_epoch) {
        Init_eval(DateTime(static_cast<time_t>(start_epoch)), DateTime(static_cast<time_t>(end_epoch)));
    }
void Run_epoch(long start_epoch, long end_epoch) {
    std::cout << "DEBUG Run_epoch start_epoch=" << start_epoch << " end_epoch=" << end_epoch << std::endl;
    DateTime d_start(static_cast<time_t>(start_epoch));
    std::cout << "DEBUG d_start.t=" << d_start.t << " year=" << d_start.year
              << " month=" << d_start.month << " day=" << d_start.day
              << " hour=" << d_start.hour << " min=" << d_start.min
              << " sec=" << d_start.sec << std::endl;
    DateTime d_end(static_cast<time_t>(end_epoch));
    std::cout << "DEBUG d_end.t=" << d_end.t << " year=" << d_end.year << std::endl;
    Run(d_start, d_end);
}
    Parameters Get_parameters() const { return *parameters; }
    Output Get_output();

    Input* Get_Input_Base_Pointer() { return input.get(); }

protected:
    double psi_leaf_init;
    double psi_stem_init;

    std::unique_ptr<Config> config;
    std::unique_ptr<Parameters> parameters;
    std::unique_ptr<Input> input;
    std::unique_ptr<Model> model;

    virtual void Analyse();
};