#include "model.h"
#include <cmath>
#include <numbers>
#include <iostream>
#include "root_distribution_model.h"
#include "soil_water_model.h"

void Leaf_Stem_Implicit_Model::Set_initial_conditions(double psi_leaf_zero, double psi_stem_zero) {
    psi_leaf = psi_leaf_zero;
    psi_stem = psi_stem_zero;
}

int Leaf_Stem_Implicit_Model::time_index(double elapsed_mins) {
    return static_cast<int>(elapsed_mins / 30.0);
}

double Leaf_Stem_Implicit_Model::update_stem_water_flow_J(double psi_leaf, double psi_stem) {

    // The factor of two reflects the water uptake from the middle of the Stem to the canopy only
    DeltaP_LS = psi_stem - psi_leaf - (params.rho_water * params.grav * params.canopy_height / 2.0) * params.PaToMPa;

    // Prevent negative pressure differences to avoid letting the water flow up the tree
    if(DeltaP_LS < 0.0)
        DeltaP_LS = 0.0;

    // Calculate the stem water flow J [mol m-2 s-1]
    // This is essentially Darcy's law
    return DeltaP_LS * params.k_xylem_sat * params.huber_value / (params.eta_LS * params.canopy_height / 2.0);

}

double Leaf_Stem_Implicit_Model::d_psi_leaf(double psi_leaf, double psi_stem) {

    // Update beta stomatal conductional parameter
    beta_stom_cond = 1.0 / (1.0 + std::exp(-params.d_50_close * (psi_leaf - params.psi_leaf_50_close)));

    // Medlyn phoytosynthesis model
    gs = params.g0  + beta_stom_cond * (1.0 + params.g1 / std::sqrt(ts_vpd / ts_pressure)) * ts_anet / ts_ca;

    J = update_stem_water_flow_J(psi_leaf, psi_stem);

    T = gs * params.leaf_area_index * ts_vpd / ts_pressure;

    return ((J - T)/ params.leaf_hydraulic_capacitance);
}

double Leaf_Stem_Implicit_Model::d_psi_stem(double psi_leaf, double psi_stem) {

    // The water stem flow could be also updated right here, but this might lead to inconsistent water uptakes/
    // in case it is updated two times.
    // J = update_stem_water_flow_J(psi_leaf, psi_stem)

    G = 0.0;

    for (int s = 0; s < params.nsoil; ++s) {
        Gi[s] = root_fractions[s] * ts_k_soil[s] * std::sqrt(params.root_area_index) /
                params.PI / params.root_zone_depth * (ts_psi_soil[s] - psi_stem -
                (params.rho_water * params.grav * params.canopy_height / 2.0) * params.PaToMPa) / params.grav * params.MPaToPa;

        // Avoid water from flowing down from the stem to the soil
        if (Gi[s] < 0.0)
            Gi[s] = 0.0;

        G += Gi[s];
    }

    return ((G - J) / (params.stem_hydraulic_capacitance * params.canopy_height * params.huber_value));
}

Leaf_Stem_Implicit_Model::Leaf_Stem_Implicit_Model
(const Parameters &parameters, const Input &input):
params(parameters),
input_module(input),
output()
{

}

void Leaf_Stem_Implicit_Model::Set_derived_parameters() {

    Root_distribution_model root_model(params);
    root_fractions = root_model.Get_rooting_fractions();

    soil_water_module = std::make_unique<Campbell_Water_Uptake>(params, input_module);

    soil_water_module->CalculatePsiAndKs();

    input_k_soil = soil_water_module->Get_ks();
    input_psi_soil = soil_water_module->Get_psi();

    input_anet = input_module.anet;
    input_vpd = input_module.vpd;

    solver_psi_leaf = std::make_unique<Bisection_psi_leaf>(*this, 1E-10, 100);
    solver_psi_stem = std::make_unique<Bisection_psi_stem>(*this, 1E-10,100);

    Gi.resize(params.nsoil);

}

void Leaf_Stem_Implicit_Model::Run(double steplength, double time_start, double time_end) {

    this->steplength = steplength;
    this->time_start = time_start;
    this->time_end = time_end;

    delta_T = time_end - time_start;
    nsteps = delta_T / steplength;


    ts = time_start;

    for (int i = 0; i < nsteps; ++i) {

        // Update forcing drivers
        ts_ca = 415.0;
        ts_pressure = 1.013*100000.0;
        ts_anet = input_anet[time_index(ts)];
        ts_vpd = input_vpd[time_index(ts)];

        ts_psi_soil = input_psi_soil[time_index(ts)];
        ts_k_soil = input_k_soil[time_index(ts)];


        const double minimum_psi_leaf = -15.0;
        double l0 = minimum_psi_leaf;
        //double s1  = *std::max_element(ts_psi_soil.begin(),ts_psi_soil.end());
        double l1 = 0.0;
        this->psi_leaf = solver_psi_leaf->Solve(l0, l1);

        // Lower bound for stem water potential is just the leaf water potential
        double s0 = psi_leaf;
        double s1 = 0.0;
        this->psi_stem = solver_psi_stem->Solve(s0, s1);

        if(psi_stem < - 7.0){
            break;
        }


        // Addind up output files
        add_output();

        // Update time step
        ts += this->steplength;
    }

    std::cout << psi_leaf << std::endl;
    std::cout << psi_stem << std::endl;
}


void Leaf_Stem_Implicit_Model::add_output() {

    output.Add_Timestep(ts);

    output.Add_T(T);
    output.Add_J(J);
    output.Add_G(G);
    vector<float> Gi_f(Gi.begin(), Gi.end());
    output.Add_G_indiv(Gi_f);


    output.Add_psi_leaf(psi_leaf);
    output.Add_psi_stem(psi_stem);

    vector<float> psi_soil_f(ts_psi_soil.begin(), ts_psi_soil.end());
    output.Add_psi_soil_indiv(psi_soil_f);

    output.Add_gs(gs);
    output.Add_beta(beta_stom_cond);

    vector<float> ks_soil_f(ts_k_soil.begin(), ts_k_soil.end());
    output.Add_ks_indiv(ks_soil_f);


    output.Add_vpd(ts_vpd);

    output.Add_steps_psi_leaf(solver_psi_leaf->Get_nsteps_converged());
    output.Add_steps_psi_stem(solver_psi_stem->Get_nsteps_converged());


}



double Leaf_Stem_Implicit_Model::psi_stem_root(double psi_stem_target) {
    double d_psi_stem_rec = d_psi_stem(psi_leaf, psi_stem_target);
    return psi_stem + d_psi_stem_rec * steplength - psi_stem_target;
}

double Leaf_Stem_Implicit_Model::psi_leaf_root(double psi_leaf_target) {
    double d_psi_leaf_rec = d_psi_leaf(psi_leaf_target, psi_stem);
    return psi_leaf + d_psi_leaf_rec * steplength - psi_leaf_target;
}

Output Leaf_Stem_Implicit_Model::Get_output() {
    return output;
}


double Bisection_psi_leaf::f(double x) {
    return model.psi_leaf_root(x);
}

double Bisection_psi_stem::f(double x) {
    return model.psi_stem_root(x);
}
