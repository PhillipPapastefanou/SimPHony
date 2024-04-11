//
// Created by Phillip on 11.07.23.
//
#pragma once
#include "../framework/parameters.h"
#include "../io/input.h"
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



// ------------------------------------------------------------------------------------------
// Soil hydraulic properties implementation based on Saxton and Rawls (2006)
// 10.2136/sssaj2005.0117
// Warning: The soil and clay fraction in the org. paper are real fractions, while only the soil
// organic matter is in percentage
// ------------------------------------------------------------------------------------------
class Saxton06_Soil_Water: public Soil_water_module {
public:
    Saxton06_Soil_Water(const Parameters& parameters, const Input& input);
    ~Saxton06_Soil_Water();
    void CalculatePsiAndKs() override;

    vector<vector<double> > Get_psi() override;
    vector<vector<double> > Get_ks() override;

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


class Van_Gnuchten_Soil_Water: public Soil_water_module{

public:
    Van_Gnuchten_Soil_Water(const Parameters& parameters, const Input& input);
    ~Van_Gnuchten_Soil_Water();
    void CalculatePsiAndKs() override;

    vector<vector<double> > Get_psi() override;
    vector<vector<double> > Get_ks() override;

};


//class VanGenuchten_Water_Uptake:


