//
// Created by Phillip on 11.07.23.
//
#pragma once
#include "parameters.h"
#include "input.h"
#include <vector>



using std::vector;



class Soil_water_module {

public:
    Soil_water_module(const Parameters& parameters, const Input& input);

    virtual ~Soil_water_module();

    virtual void CalculatePsiAndKs() = 0;
    virtual vector<vector<double> > Get_psi() = 0;
    virtual vector<vector<double> > Get_ks() = 0;


protected:
    const Parameters& parameters;
    const Input& input_module;

    /// Hydraulic conductivity per soil layer [m s-1]
    vector<vector<double> > ks_array;
    /// Matric potential of each soil layer
    vector<vector<double> > psi_s_array;

};

class Campbell_Soil_Water: public Soil_water_module {
public:
    Campbell_Soil_Water(const Parameters& parameters, const Input& input);
    ~Campbell_Soil_Water();
    void CalculatePsiAndKs() override;

    vector<vector<double> > Get_psi() override;
    vector<vector<double> > Get_ks() override;

};

class Saxton06_Soil_Water: public Soil_water_module {
public:
    Saxton06_Soil_Water(const Parameters& parameters, const Input& input);
    ~Saxton06_Soil_Water();
    void CalculatePsiAndKs() override;

    vector<vector<double> > Get_psi() override;
    vector<vector<double> > Get_ks() override;

private:
    double calc_theta_s();

    double calc_theta_33_t();
    double calc_theta_33();

    double calc_theta_s_33_t();
    double calc_theta_s_33();

    double calc_theta_1500_t();
    double calc_theta_1500();

    double calc_A(double B);
    double calc_B();
    double calc_KS();

    double theta_s;

    double theta_33;
    double theta_t_33;
    double theta_s_33;
    double theta_s_33_t;

    double theta_1500;
    double theta_1500t;

    double A;
    double B;
    double Ks;

    double clay_frac;
    double sand_frac;
    double orgmat_rac;

};


class Van_Gnuchten_Soil_Water: public Soil_water_module{

public:
    Van_Gnuchten_Soil_Water(const Parameters& parameters, const Input& input);
    ~Van_Gnuchten_Soil_Water();
    void CalculatePsiAndKs() override;

    vector<vector<double> > Get_psi() override;
    vector<vector<double> > Get_ks() override;

};


//class VanGenuchten_Water_Uptake:


