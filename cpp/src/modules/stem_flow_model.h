//
// Created by ppapastefanou on 4/8/24.
//

#pragma once
#include "../framework/parameters.h"
#include "../auxil/lookup_table.h"

class Stem_flow_module {

public:
    Stem_flow_module(const Parameters& params);
    virtual ~Stem_flow_module();


    virtual void Init() = 0;
    virtual double Get_Stem_flow(double psi_stem, double psi_leaf) = 0;
    void Update_min_conductivity_fractions();

protected:
    const Parameters& params;
    std::vector<double> min_frac_con_per_segment;
    std::vector<double> actual_frac_con_per_segment;

};


class Linear_stem_flow : public Stem_flow_module{

public:
    Linear_stem_flow(const Parameters& params);
    void Init() override;
    double Get_Stem_flow(double psi_root, double psi_leaf) override;

private:
    double b;
    double c;
    Lookup_table k_xylem_loss_table;

};


class Kirchhoff_Weibull_stem_flow : public Stem_flow_module{

public:
    Kirchhoff_Weibull_stem_flow(const Parameters& params);
    void Init() override;
    double Get_Stem_flow(double psi_stem, double psi_leaf) override;

private:
    double b;
    double c;

    double KirchhoffIntegral(double psi);
};

class Kirchhoff_Piecewise_Erf : public Stem_flow_module{

public:
    Kirchhoff_Piecewise_Erf(const Parameters& params);
    void Init() override;
    double Get_Stem_flow(double psi_stem, double psi_leaf) override;

private:
    double b;
    double c;

    double q95;
    double psi_95;
    double q05;
    double psi_05;

    double slope_up;
    double slope_low;

    double KirchhoffIntegral(double psi, double s);
    double KirchhoffIntegralSplit(double psi);

    double FindKirchhoffSlope(double psi50, double psi_q, double q);
    double g_erf(double psi50, double slope, double psi_q, double q);
};