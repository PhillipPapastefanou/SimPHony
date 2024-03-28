//
// Created by Phillip on 11.07.23.
//

#include "soil_water_model.h"
#include <iostream>

Soil_water_module::Soil_water_module(const Parameters& parameters, const Input& input):
parameters(parameters), input_module(input){

}

Soil_water_module::~Soil_water_module() {

}

void Campbell_Soil_Water::CalculatePsiAndKs() {

    const double b = parameters.camp_b;
    const double Ks = parameters.k_soil_sat;
    const double theta_s = parameters.theta_s;
    const double psi_s_ref = parameters.camp_psi_soil_ref;
    int nsoil = parameters.soil_depths.size();

    vector<vector<float> > theta_array = input_module.theta_per_layer;
    this->psi_s_array.resize(theta_array.size());
    this->ks_array.resize(theta_array.size());


    for (int i = 0; i < theta_array.size(); ++i) {

        vector<float> theta_list = theta_array[i];


        // Resize all theta water contents of each layers

        for (float& wcont : theta_list) {
            wcont *= parameters.theta_emp_multiplier;
        }


        int navail_soil_in_data = theta_list.size();

        vector<double> psi_row(nsoil);
        vector<double> k_row(nsoil);


        int s_h = 0;
        // The loop also reverse the layers to make the top layer be layer one.
        for (int s = navail_soil_in_data - 1; s > navail_soil_in_data - 1 - nsoil  ; --s) {

            double base = theta_list[s]/theta_s;
            if((base < 0.0) || (base > 1.0)){
                std::cout << "Invalid water content or theta_s paramter: ";
                std::cout << "Theta(t) is " << theta_list[s];
                std::cout << " and Theta_s is " << theta_s;
                std::cout << ". Exiting simulation..." << std::endl;
                exit(99);
            }

            psi_row[s_h] = psi_s_ref *  std::pow(theta_list[s]/theta_s, -b);
            k_row[s_h] = Ks * std::pow(theta_list[s] / theta_s, 2.0 + 3.0 * b);

            s_h++;
        }

        psi_s_array[i] = psi_row;
        ks_array[i] = k_row;

    }
}

vector<vector<double> > Campbell_Soil_Water::Get_psi() {
    return psi_s_array;
}

vector<vector<double> > Campbell_Soil_Water::Get_ks() {
    return ks_array;
}

Campbell_Soil_Water::Campbell_Soil_Water(const Parameters& parameters, const Input& input): Soil_water_module(parameters, input)
{

}

Campbell_Soil_Water::~Campbell_Soil_Water() {

}

Saxton06_Soil_Water::Saxton06_Soil_Water(const Parameters &parameters, const Input &input) : Soil_water_module(
        parameters, input) {

}

void Saxton06_Soil_Water::CalculatePsiAndKs() {

    vector<vector<float> > theta_array = input_module.theta_per_layer;
    this->psi_s_array.resize(theta_array.size());
    this->ks_array.resize(theta_array.size());
    int nsoil = parameters.soil_depths.size();

    if(nsoil != theta_array.front().size()){
        std::cout << "Numbers of soil layers does not match number of layers in input";
        exit(99);
    }

    sand_frac = parameters.sand_frac;
    clay_frac = parameters.clay_frac;
    orgmat_rac = parameters.organic_matter_frac;

    theta_t_33      = calc_theta_33_t();
    theta_33        = calc_theta_33();
    theta_s_33_t    = calc_theta_s_33_t();
    theta_s_33      = calc_theta_s_33();
    theta_s         = calc_theta_s();

    theta_1500t     = calc_theta_1500_t();
    theta_1500      = calc_theta_1500();

    B       = calc_B();
    A       = calc_A(B);
    //Todo make an option to include the calculation of KS
    //Ks      = calc_KS();
    // // Convert from mm d-1 to m s-1 Saxton 2006 is in mm d-1)
    // Ks /= (3600*1000);

    Ks = parameters.k_soil_sat;

    // Convert from pressure head m to MPa
    Ks *= 1000.0/parameters.grav;

    // Convert from kg to mol H2O
    Ks *= 1000.0/18.0;


    for (int i = 0; i < theta_array.size(); ++i) {

        vector<float> theta_list = theta_array[i];
        // Resize all theta water contents of each layers

        for (float& wcont : theta_list) {
            wcont *= parameters.theta_emp_multiplier;
        }


        int navail_soil_in_data = theta_list.size();

        vector<double> psi_row(nsoil);
        vector<double> k_row(nsoil);

        int s_h = 0;
        // The loop also reverse the layers to make the top layer be layer one.
        for (int s = 0; s <  nsoil  ; ++s) {
//        for (int s = navail_soil_in_data - 1; s > navail_soil_in_data - 1 - nsoil  ; --s) {

            double base = theta_list[s]/theta_s;
            if((base < 0.0) || (base > 1.0)){
                std::cout << "Invalid water content or theta_s paramter: ";
                std::cout << "Theta(t) is " << theta_list[s];
                std::cout << " and Theta_s is " << theta_s;
                std::cout << ". Exiting simulation..." << std::endl;
                exit(99);
            }

            // Add a minus because water potentials are per definition negative
            psi_row[s_h] = - A *  std::pow(theta_list[s]/theta_s, -B);

            // Devide by 1000 to get from kPA to MPa
            psi_row[s_h] /= 1000.0;

            k_row[s_h] = Ks *  std::pow(theta_list[s]/theta_s, 3.0 + 2.0 * B);

            s_h++;
        }

        psi_s_array[i] = psi_row;
        ks_array[i] = k_row;

    }





}

double Saxton06_Soil_Water::calc_theta_33_t() {

    const double sf = sand_frac;
    const double cf = clay_frac;
    const double of = orgmat_rac;
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction
    double d1 = -0.251 * sf + 0.195*cf + 0.011*ofp;
    double d2 = 0.006* sf * ofp - 0.027*cf*ofp + 0.452*cf*sf;
    return d1 + d2 + 0.299;
}

double Saxton06_Soil_Water::calc_theta_33() {

    const double th33t = theta_t_33;

    return th33t + 1.283*th33t*th33t - 0.374*th33t - 0.015;
}


double Saxton06_Soil_Water::calc_theta_s_33_t() {

    const double sf = sand_frac;
    const double cf = clay_frac;
    const double of = orgmat_rac;
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction
    double d1 = 0.278 * sf + 0.034*cf + 0.022*ofp;
    double d2 = -0.018* sf * ofp - 0.027*cf*ofp - 0.584*cf*sf;
    return d1 + d2 + 0.078;
}

double Saxton06_Soil_Water::calc_theta_s_33() {

    const double th33mt = theta_s_33_t;

    return th33mt + 0.636*th33mt - 0.107;
}

double Saxton06_Soil_Water::calc_theta_s() {
    return theta_33 + theta_s_33 - 0.097*sand_frac + 0.043;
}


double Saxton06_Soil_Water::calc_theta_1500_t() {

    const double sf = sand_frac;
    const double cf = clay_frac;
    const double of = orgmat_rac;
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction

    double d1 = -0.024 * sf + 0.487*cf + 0.006*ofp;
    double d2 = 0.005* sf * ofp - 0.013*cf*ofp + 0.068*cf*sf;
    return d1 + d2 + 0.031;
}

double Saxton06_Soil_Water::calc_theta_1500() {

    const double th1500t = theta_1500t;
    return th1500t + 0.14*th1500t - 0.02;
}

double Saxton06_Soil_Water::calc_B() {
    double en = std::log(1500.0) - std::log(33.0);
    double den = std::log(theta_33) - std::log(theta_1500);
    return en/den;
}

double Saxton06_Soil_Water::calc_A(double B) {
    return std::exp(std::log(33.0) + B*std::log(theta_33));
}


double Saxton06_Soil_Water::calc_KS() {
    const double L = 1.0/B;
    return 1930 * std::pow(theta_s - theta_33, 3.0 - L);
}

Saxton06_Soil_Water::~Saxton06_Soil_Water() {

}
vector<vector<double> > Saxton06_Soil_Water::Get_psi() {
    return psi_s_array;
}

vector<vector<double> > Saxton06_Soil_Water::Get_ks() {
    return ks_array;
}

Van_Gnuchten_Soil_Water::Van_Gnuchten_Soil_Water(const Parameters &parameters, const Input &input)
        : Soil_water_module(parameters, input) {

}

Van_Gnuchten_Soil_Water::~Van_Gnuchten_Soil_Water() {

}


vector<vector<double> > Van_Gnuchten_Soil_Water::Get_psi() {
    return psi_s_array;
}

vector<vector<double> > Van_Gnuchten_Soil_Water::Get_ks() {
    return ks_array;
}

void Van_Gnuchten_Soil_Water::CalculatePsiAndKs() {

    vector<vector<float> > theta_array = input_module.theta_per_layer;
    psi_s_array.resize(theta_array.size());
    ks_array.resize(theta_array.size());
    int nsoil = parameters.soil_depths.size();

    if(nsoil != theta_array.front().size()){
        std::cout << "Numbers of soil layers does not match number of layers in input";
        exit(99);
    }

    const double g       = parameters.grav;
    const double alpha   = parameters.alpha_genucht;
    const double n       = parameters.n_genucht;
    const double eta     = parameters.neta_genucht;
    const double theta_s = parameters.theta_s;
    const double theta_r = parameters.theta_r;
    const double m       = 1.0 - 1.0 / n;

    for (int i = 0; i < theta_array.size(); ++i) {

        vector<float> theta_list = theta_array[i];
        // Resize all theta water contents of each layers

        for (float& wcont : theta_list) {
            wcont *= parameters.theta_emp_multiplier;
        }

        int navail_soil_in_data = theta_list.size();

        vector<double> psi_row(nsoil);
        vector<double> k_row(nsoil);

        int s_h = 0;
        // The loop also reverse the layers to make the top layer be layer one.
        for (int s = 0; s <  nsoil  ; ++s) {

            double se = (-theta_list[s] + theta_r)/(theta_r - theta_s);

            if((se < 0.0) || (se > 1.0)){
                std::cout << "Invalid water content or theta_s parameter: ";
                std::cout << "Theta(t) is " << theta_list[s];
                std::cout << " and Theta_s is " << theta_s;
                std::cout << " and Theta_r is " << theta_r;
                std::cout << ". Exiting simulation..." << std::endl;
                exit(99);
            }

            psi_row[s_h] = 1.0/1000.0 * std::pow(se, -1.0 / m) * ( -1.0 + std::pow(se, 1.0 / m)) * std::pow(alpha, -n);

            double Ks = parameters.k_soil_sat;
            // Convert from pressure head m to MPa
            Ks *= 1000.0/g;

            // Convert from kg to mol H2O
            Ks *= 1000.0/18.0;

            double u = (1.0 - (1.0 - std::pow(std::pow(se, 1.0/m), m)));
            double d = std::pow(se, eta) * u * u;
            k_row[s_h] = Ks *  d;

            s_h++;
        }

        psi_s_array[i] = psi_row;
        ks_array[i] = k_row;

    }

}
