//
// Created by Phillip on 03/09/2024.
//

#pragma once
#include <vector>
#include "soil_water_model.h"

// ------------------------------------------------------------------------------------------
// Soil hydraulic properties implementation based on Saxton and Rawls (2006)
// 10.2136/sssaj2005.0117
// Warning: The sand and clay fraction in the org. paper are real fractions, while only the soil
// organic matter is in percentage
// Todo reenable Ks calculation
// ------------------------------------------------------------------------------------------
class Saxton06: public Soil_water_module {
public:
    Saxton06(const Parameters& parameters, const Input& input, const Config& config);
    ~Saxton06();
    void CalculatePsiAndKs(int start_idx = 0, int end_idx = -1) override;

private:
    double calc_theta_s(int si);

    double calc_theta_33_t(int si);
    double calc_theta_33(int si);

    double calc_theta_s_33_t(int si);
    double calc_theta_s_33(int si);

    double calc_theta_1500_t(int si);
    double calc_theta_1500(int si);

    double calc_A(int si, double B);
    double calc_B(int si);
    double calc_KS(int si);

    vector<double> theta_s;

    vector<double> theta_33;
    vector<double> theta_t_33;
    vector<double> theta_s_33;
    vector<double> theta_s_33_t;

    vector<double> theta_1500;
    vector<double> theta_1500t;

    vector<double> A;
    vector<double> B;
    vector<double> Ks;

    vector<double> clay_fracs;
    vector<double> sand_fracs;
    vector<double> orgmat_fracs;

};
