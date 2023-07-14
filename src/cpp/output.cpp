//
// Created by Phillip on 12.07.23.
//

#include "output.h"

Output::Output() {

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

void Output::Add_Timestep(double t) {
    times.push_back(t);
}

void Output::Add_steps_psi_stem(int steps_psi_stem) {
    steps_psi_stem_a.push_back(steps_psi_stem);
}

void Output::Add_steps_psi_leaf(int steps_psi_leaf) {
    steps_psi_leaf_a.push_back(steps_psi_leaf);
}
