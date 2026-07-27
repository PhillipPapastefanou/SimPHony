//
// Prognostic soil water balance: precipitation infiltration + vertical Richards-equation
// transport between soil layers + root water uptake sink, as an alternative to reading
// soil moisture directly from the observed forcing time series (Soil_water_module).
//
// Structured after JSBACH4's mo_hydro_process.f90 (soilhyd / calc_vertical_transport /
// get_soilhyd_properties, "Uniform_" scale infiltration scheme), with snow/ice/canopy
// skin reservoir/pond/permafrost machinery left out -- see the accompanying plan
// (declarative-churning-tulip.md) for the full derivation and the reasoning for
// implementing this as a from-scratch discretization of the governing equation rather
// than a line-by-line port of the (much more general, vectorized) Fortran routine.
//
#pragma once
#include <vector>
#include "soil_water_model.h"

class Soil_hydrology_richards {

public:
    Soil_hydrology_richards(const Parameters& parameters, const Input& input, const Config& config,
                             const Soil_water_module& curve_provider);

    /// Sets the initial per-layer volumetric water content [m3 m-3] (e.g. the first
    /// observed-forcing theta row, used as a warm start) and derives the initial psi/k state.
    void Init(const std::vector<float>& theta0);

    /// Current per-layer state (as of the last Init()/Step() call): matric potential expressed
    /// as hydraulic head [m], hydraulic conductivity [m s-1], volumetric water content [m3 m-3].
    void Get_state(std::vector<double>& psi_soil, std::vector<double>& k_soil,
                   std::vector<float>& theta) const;

    /// Advances the soil moisture state by one timestep: partitions precip_rate [kg m-2 s-1]
    /// into infiltration/runoff, solves the implicit vertical Richards transport with
    /// root_uptake [mol H2O m-2 s-1 per layer, per unit GROUND area -- NOT the per-leaf-area
    /// basis Water_Potential_Solver::Get_root_uptake_indiv() returns; Model::Run() converts
    /// before calling this] as a sink term, and updates the internal state (queried
    /// afterwards via Get_state()) as well as the infiltration/runoff/drainage diagnostics
    /// below.
    void Step(double precip_rate, const std::vector<double>& root_uptake, double dt);

    /// Diagnostics from the most recent Step() call, all in [kg m-2 s-1] (same convention as
    /// Input::precip) for direct use as Output series.
    double Get_infiltration() const { return infiltration_kg_m2_s1; }
    double Get_runoff() const { return runoff_kg_m2_s1; }
    double Get_drainage() const { return drainage_kg_m2_s1; }

private:
    const Parameters& params;
    const Input& input;
    const Config& config;
    const Soil_water_module& curve_provider;

    int nsoil;

    /// Volumetric water content per soil layer [m3 m-3] -- the prognostic state.
    std::vector<double> theta;
    /// Cached matric potential (hydraulic head [m]) and conductivity ([m s-1]) for the current
    /// theta state, recomputed on Init()/after every Step().
    std::vector<double> psi_cache;
    std::vector<double> k_cache;

    double infiltration_kg_m2_s1 = 0.0;
    double runoff_kg_m2_s1 = 0.0;
    double drainage_kg_m2_s1 = 0.0;

    void update_psi_k_cache();
};
