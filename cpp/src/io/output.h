//
// Created by Phillip on 12.07.23.
//

#pragma once
#include <vector>
#include <string>
#include "../framework/date_time.h"
#include "../framework/parameters.h"

using std::vector;
class Output {

public:
    Output(const Parameters& parameters);
    ~Output();

    void Add_Timestep(double t);
    void Add_DateTime(DateTime t);

    void Add_T(double t);
    void Add_J(double J);
    void Add_G(double G);

    void Add_G_indiv(vector<float> G_indiv);

    void Add_psi_leaf(float psi_leaf);
    void Add_psi_stem_ground(float psi_stem_ground);
    void Add_psi_stems_seg(vector<float> psi_stem_seg);
    void Add_psi_soil_indiv(vector<float> psi_soil_indiv);

    void Add_gs( double gs);
    void Add_beta(double beta);
    void Add_ks_indiv(vector<float> ks_indiv);
    void Add_vpd(double vpd);
    void Add_anet(double anet);

    void Add_steps_psi_leaf(int steps_psi_leaf);
    void Add_steps_psi_stem_ground(int steps_psi_stem);

    const vector<DateTime> &Get_dates() const;
    const vector<long> &Get_times() const;
    const vector<float> &Get_T() const;
    const vector<float> &Get_J() const;
    const vector<float> &Get_G() const;

    vector<float> Get_J_per_sap() const;
    vector<float> Get_G_per_sap() const;

    const vector<vector<float> > &Get_G_indiv() const;

    const vector<float> &Get_psi_leaf() const;

    const vector<float> &Get_psi_stem() const;

    const vector<vector<float> > &Get_psi_soil_indiv() const;

    const vector<float> &Get_gs() const;

    const vector<float> &Get_beta() const;

    const vector<float> &Get_vpd() const;

    const vector<float> &Get_anet() const;


    const vector<vector<float> > &Get_ks_soil() const;

    const vector<int> &Get_steps_psi_leaf() const;

    const vector<int> &Get_steps_psi_stem() const;

    void Export_CSV(std::string filename);


private:
    const Parameters& parameters;

    std::vector<long> times;
    std::vector<DateTime> dates;

    vector<float> Ta;
    vector<float> Ja;
    vector<float> Ga;

    vector<vector<float> > Gaa;

    vector<float> psi_leaf_a;
    vector<float> psi_stem_ground_a;
    vector<vector<float> > psi_stem_a;
    vector<vector<float> > psi_soil_aa;

    vector<float> gs_a;

    vector<float> beta_a;

    vector<float> vpd_a;
    vector<float> anet_a;

    vector<vector<float> > k_soil_aa;

    vector<int> steps_psi_leaf_a;
    vector<int> steps_psi_stem_a;




};


