//
// Created by Phillip on 12.07.23.
//

#include "output.h"
#include <iostream>

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

void Output::Add_G_indiv(vector<float> G_indiv) {
    Gaa.push_back(G_indiv);
}

void Output::Add_psi_leaf(float psi_leaf) {
    psi_leaf_a.push_back(psi_leaf);
}

void Output::Add_psi_stem(float psi_stem) {
    psi_stem_a.push_back(psi_stem);
}

void Output::Add_psi_soil_indiv(vector<float> psi_soil_indiv) {
    psi_soil_aa.push_back(psi_soil_indiv);
}

void Output::Add_gs(double gs) {
    gs_a.push_back(gs);
}

void Output::Add_beta(double beta) {
    beta_a.push_back(beta);
}

void Output::Add_ks_indiv(vector<float> ks_indiv) {
    k_soil_aa.push_back(ks_indiv);
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

void Output::Add_steps_psi_stem(int steps_psi_stem) {
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

const vector<vector<float>> &Output::Get_G_indiv() const {
    return Gaa;
}

const vector<float> &Output::Get_psi_leaf() const {
    return psi_leaf_a;
}

const vector<float> &Output::Get_psi_stem() const {
    return psi_stem_a;
}

const vector<vector<float> > &Output::Get_psi_soil_indiv() const {
    return psi_soil_aa;
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

const vector<vector<float> > &Output::Get_ks_soil() const {
    return k_soil_aa;
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



