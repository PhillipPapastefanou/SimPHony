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

    if (parameters.k_soil_sats.size() != 1){
        std::cout << "Campbell soil module only works for a single conductivity 1" ;
        exit(99);
    }

    std::cout << "Warning: The Campbell soil module is not properly implemented. Please check the equations" << std::endl;
    const double b = parameters.camp_b;
    const double Ks = parameters.k_soil_sats[0];
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

    int number_of_layers_in_input = theta_array[0].size();
    int nsoil = parameters.soil_depths.size();

    if(nsoil != number_of_layers_in_input){
        std::cout << "Numbers of soil layers does not match number of layers in input";
        exit(99);
    }

    if (parameters.sand_fracs.size() != nsoil){
        std::cout << "Invalid number of sand fracs supplied: " << parameters.sand_fracs.size();
        exit(99);
    }

    if (parameters.clay_fracs.size() != nsoil){
        std::cout << "Invalid number of clay fracs supplied: " << parameters.sand_fracs.size();
        exit(99);
    }

    if (parameters.organic_matter_fracs.size() != nsoil){
        std::cout << "Invalid number of organic_matter_fracs supplied: " << parameters.sand_fracs.size();
        exit(99);
    }

    if (parameters.k_soil_sats.size() != nsoil){
        std::cout << "Invalid number of soil hydraulic conductivities supplied: " << parameters.sand_fracs.size();
        exit(99);
    }

    Ks = parameters.k_soil_sats;

    sand_fracs = parameters.sand_fracs;
    clay_fracs = parameters.clay_fracs;
    orgmat_fracs = parameters.organic_matter_fracs;

    theta_t_33.resize(nsoil);
    theta_33.resize(nsoil);
    theta_s_33_t.resize(nsoil);
    theta_s_33.resize(nsoil);
    theta_s.resize(nsoil);
    theta_1500t.resize(nsoil);
    theta_1500.resize(nsoil);
    B.resize(nsoil);
    A.resize(nsoil);

    for (int is = 0; is < nsoil; ++is) {
        theta_t_33[is] = calc_theta_33_t(is);

        theta_t_33[is]      = calc_theta_33_t(is);
        theta_33[is]        = calc_theta_33(is);
        theta_s_33_t[is]    = calc_theta_s_33_t(is);
        theta_s_33[is]      = calc_theta_s_33(is);
        theta_s[is]         = calc_theta_s(is);

        theta_1500t[is]     = calc_theta_1500_t(is);
        theta_1500[is]      = calc_theta_1500(is);

        B[is]       = calc_B(is);
        A[is]       = calc_A(is, B[is]);
    }



    //Todo make an option to include the calculation of KS
    //Ks      = calc_KS();
    // // Convert from mm d-1 to m s-1 Saxton 2006 is in mm d-1)
    // Ks /= (3600*1000);

    for (int is = 0; is < nsoil; ++is) {
        // Convert from pressure head m to MPa
        Ks[is] *= 1000.0/parameters.grav;

        //Account for the density of water
        // 1 m = 1000 kg m-2
        Ks[is] *= 1000.0;

        // Convert from kg to mol H2O
        Ks[is] *= 1000.0/18.0;
    }




    for (int i = 0; i < theta_array.size(); ++i) {

        vector<float> theta_list = theta_array[i];
        // Resize all theta water contents of each layers

        for (float& wcont : theta_list) {
            wcont *= parameters.theta_emp_multiplier;
        }


        int navail_soil_in_data = theta_list.size();

        vector<double> psi_row(nsoil);
        vector<double> k_row(nsoil);

        // The loop also reverse the layers to make the top layer be layer one.
        // Top layer must always be the layer 0
        for (int s = 0; s <  nsoil ; ++s) {

            double base = theta_list[s]/ theta_s[s];
            if(base < 0.0){
                std::cout << "Invalid water content or theta_s paramter: ";
                std::cout << "Theta(t) is " << theta_list[s];
                std::cout << " and Theta_s is " << theta_s[s];

                std::cout << ". Exiting simulation..." << std::endl;
                exit(99);
            }

            // Fully saturated water
            if (base > 1.0){
                base = 1.0;
            }

            // Add a minus in fromt of A because water potentials are per definition negative
            psi_row[s] = - A[s] *  std::pow(base, -B[s]);

            // Devide by 1000 to get from kPA to MPa
            psi_row[s] /= 1000.0;

            k_row[s] = Ks[s] *  std::pow(base, 3.0 + 2.0 * B[s]);
        }

        psi_s_array[i] = psi_row;
        ks_array[i] = k_row;

    }


}

double Saxton06_Soil_Water::calc_theta_33_t(int si) {

    const double sf = sand_fracs[si];
    const double cf = clay_fracs[si];
    const double of = orgmat_fracs[si];
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction
    double d1 = -0.251 * sf + 0.195*cf + 0.011*ofp;
    double d2 = 0.006* sf * ofp - 0.027*cf*ofp + 0.452*cf*sf;
    return d1 + d2 + 0.299;
}

double Saxton06_Soil_Water::calc_theta_33(int is) {

    const double th33t = theta_t_33[is];

    return th33t + 1.283*th33t*th33t - 0.374*th33t - 0.015;
}


double Saxton06_Soil_Water::calc_theta_s_33_t(int is) {

    const double sf = sand_fracs[is];
    const double cf = clay_fracs[is];
    const double of = orgmat_fracs[is];
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction
    double d1 = 0.278 * sf + 0.034*cf + 0.022*ofp;
    double d2 = -0.018* sf * ofp - 0.027*cf*ofp - 0.584*cf*sf;
    return d1 + d2 + 0.078;
}

double Saxton06_Soil_Water::calc_theta_s_33(int is) {

    const double th33mt = theta_s_33_t[is];

    return th33mt + 0.636*th33mt - 0.107;
}

double Saxton06_Soil_Water::calc_theta_s(int is) {
    return theta_33[is] + theta_s_33[is] - 0.097 * sand_fracs[is] + 0.043;
}


double Saxton06_Soil_Water::calc_theta_1500_t(int is) {

    const double sf = sand_fracs[is];
    const double cf = clay_fracs[is];
    const double of = orgmat_fracs[is];
    const double HPC = 100.0;
    const double ofp = of*HPC;

    // Note: WE are using the org paper values here, where sand and clay are
    // in fractions and NOT in %. Only organic matter is correctly display in fraction

    double d1 = -0.024 * sf + 0.487*cf + 0.006*ofp;
    double d2 = 0.005* sf * ofp - 0.013*cf*ofp + 0.068*cf*sf;
    return d1 + d2 + 0.031;
}

double Saxton06_Soil_Water::calc_theta_1500(int is) {

    const double th1500t = theta_1500t[is];
    return th1500t + 0.14*th1500t - 0.02;
}

double Saxton06_Soil_Water::calc_B(int is) {
    double en = std::log(1500.0) - std::log(33.0);
    double den = std::log(theta_33[is]) - std::log(theta_1500[is]);
    return en/den;
}

double Saxton06_Soil_Water::calc_A(int is, double B) {
    return std::exp(std::log(33.0) + B*std::log(theta_33[is]));
}


double Saxton06_Soil_Water::calc_KS(int is) {
    const double L = 1.0/B[is];
    return 1930 * std::pow(theta_s[is] - theta_33[is], 3.0 - L);
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

            // Todo reenable to account for differents sats
            double Ks = parameters.k_soil_sats[0];
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
