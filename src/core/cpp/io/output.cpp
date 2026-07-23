//
// Created by Phillip on 12.07.23.
//

#include "output.h"
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>

using emscripten::val;
using emscripten::typed_memory_view;
#endif
Output::Output(const Parameters& parameters):parameters(parameters) {

}

Output::~Output() {

}

void Output::Add_J(double J) {
    Ja.push_back(J);
}

void Output::Add_T(double ta) {
    Ta.push_back(ta);
}

void Output::Add_G(double G) {
    Ga.push_back(G);
}

void Output::Add_O(double O) {
    Oa.push_back(O);
}

void Output::Add_G_indiv(const vector<double>& G_indiv, double scale) {
    Gaa.Add_row(G_indiv, scale);
}

void Output::Add_psi_leaf(float psi_leaf) {
    psi_leaf_a.push_back(psi_leaf);
}

void Output::Add_J_adapted_density(double J) {

    double J_adapted = J;
    J_adapted *= 1000.0;
    J_adapted /= parameters.leaf_area_index;
    J_adapted *= parameters.huber_value;
    J_adapted /= parameters.tree_density;
    Ja_adapted.push_back(J_adapted);
}

void Output::Add_psi_sap_ground(float psi_root) {
    psi_sap_ground_a.push_back(psi_root);
}

void Output::Add_psi_heart_ground(float psi_root) {
    psi_heart_ground_a.push_back(psi_root);
}

void Output::Add_psi_stems_seg(const vector<double>& psi_stem_seg, double scale) {
    psi_stem_a.Add_row(psi_stem_seg, scale);
}



void Output::Add_psi_soil_indiv(const vector<double>& psi_soil_indiv, double scale) {
    psi_soil_aa.Add_row(psi_soil_indiv, scale);
}

void Output::Add_gs(double gs) {
    gs_a.push_back(gs);
}

void Output::Add_beta(double beta) {
    beta_a.push_back(beta);
}

void Output::Add_ks_indiv(const vector<double>& ks_indiv, double scale) {
    k_soil_aa.Add_row(ks_indiv, scale);
}

void Output::Add_vpd(double vpd) {
    vpd_a.push_back(vpd);
}

void Output::Add_anet(double anet) {
    anet_a.push_back(anet);
}

void Output::Add_Timestep(double t) {
    times.push_back(t);
}

void Output::Add_steps_psi_stem_ground(int steps_psi_stem) {
    steps_psi_stem_a.push_back(steps_psi_stem);
}

void Output::Add_steps_psi_leaf(int steps_psi_leaf) {
    steps_psi_leaf_a.push_back(steps_psi_leaf);
}



const vector<long> &Output::Get_times() const {
    return times;
}

const vector<float> &Output::Get_J() const {
    return Ja;
}

const vector<float> &Output::Get_G() const {
    return Ga;
}

const vector<float> &Output::Get_O() const {
    return Oa;
}


vector<vector<float>> Output::Get_G_indiv() const {
    return Gaa.To_nested();
}

const vector<float> &Output::Get_psi_leaf() const {
    return psi_leaf_a;
}

const vector<float> &Output::Get_psi_sap() const {
    return psi_sap_ground_a;
}

const vector<float> &Output::Get_psi_heart() const {
    return psi_heart_ground_a;
}

vector<vector<float> > Output::Get_psi_soil_indiv() const {
    return psi_soil_aa.To_nested();
}

const vector<float> &Output::Get_gs() const {
    return gs_a;
}

const vector<float> &Output::Get_beta() const {
    return beta_a;
}

const vector<float> &Output::Get_vpd() const {
    return vpd_a;
}

vector<vector<float> > Output::Get_ks_soil() const {
    return k_soil_aa.To_nested();
}

const vector<int> &Output::Get_steps_psi_leaf() const {
    return steps_psi_leaf_a;
}

const vector<int> &Output::Get_steps_psi_stem() const {
    return steps_psi_stem_a;
}

const vector<float> &Output::Get_T() const {
    return Ta;
}

const vector<float> &Output::Get_anet() const {
    return anet_a;
}



void Output::Export_CSV(std::string filename) {

    std::cout << "CSV export is not yet available!";
    throw;
}

void Output::Add_DateTime(DateTime t) {
    dates.push_back(t);
}

vector<float> Output::Get_J_per_sap() const {

    std::vector<float> Ja_td(Ja.begin(), Ja.end());
    for(auto& J : Ja_td){
        J /= parameters.tree_density;
    }
    return Ja_td;
}

vector<float> Output::Get_G_per_sap() const {
    std::vector<float> Ga_td(Ga.begin(), Ga.end());
    for(auto& G : Ga_td){
        G /= parameters.tree_density;
    }
    return Ga_td;
}

const vector<DateTime> &Output::Get_dates() const {
    return dates;
}

vector<float> Output::Get_J_per_area() const {
    return Ja_adapted;
}

#ifdef __EMSCRIPTEN__
val Output::Get_T_view() const {
    return val(typed_memory_view(Ta.size(), Ta.data()));
}
val Output::Get_J_view() const {
    return val(typed_memory_view(Ja.size(), Ja.data()));
}
val Output::Get_psi_leaf_view() const {
    return val(typed_memory_view(psi_leaf_a.size(), psi_leaf_a.data()));
}
val Output::Get_psi_sap_view() const {
    return val(typed_memory_view(psi_sap_ground_a.size(), psi_sap_ground_a.data()));
}
val Output::Get_gs_view() const {
    return val(typed_memory_view(gs_a.size(), gs_a.data()));
}
val Output::Get_vpd_view() const {
    return val(typed_memory_view(vpd_a.size(), vpd_a.data()));
}
val Output::Get_beta_view() const {
    return val(typed_memory_view(beta_a.size(), beta_a.data()));
}
#endif

void Output::Clear() {
    times.clear();
    dates.clear();
    Ta.clear();
    Ja.clear();
    Ja_adapted.clear();
    Ga.clear();
    Gaa.clear();
    Oa.clear();
    psi_leaf_a.clear();
    psi_sap_ground_a.clear();
    psi_heart_ground_a.clear();
    psi_stem_a.clear();
    psi_soil_aa.clear();
    gs_a.clear();
    beta_a.clear();
    vpd_a.clear();
    anet_a.clear();
    k_soil_aa.clear();
    steps_psi_leaf_a.clear();
    steps_psi_stem_a.clear();
}







