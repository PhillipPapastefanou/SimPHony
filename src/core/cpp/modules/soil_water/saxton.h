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

    // Prognostic soil hydrology (Soil_hydrology_richards) is not supported with the
    // Saxton06 pedotransfer function yet -- its coefficients (A, B, KS, ...) are derived
    // once per observed-forcing timestep from sand/clay/organic-matter fractions, not
    // from a simple closed-form psi(theta)/K(theta) the Richards solver could re-evaluate
    // every layer every timestep. Both calls exit(99) with a clear message.
    double Psi_from_theta(double theta, const Soil_layer& layer) const override;
    double K_from_theta(double theta, const Soil_layer& layer) const override;

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
