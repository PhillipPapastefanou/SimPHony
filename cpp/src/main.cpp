#include "../Tests/swiss_single_test.h"
#include "../Tests/hainich_single_test.h"
#include "../Tests/multi_test.h"
#include "framework/date_time.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include "modules/stem_flow_model.h"
#include <vector>

int main(int argc, char* argv[]) {

//    std::string s = "01-01-1900 00:30:00";
//
//    std::string format = "%d-%m-%Y %H:%M:%S";
//
//    DateTime dt0(s, format);
//    DateTime dt(dt0, 1800);
//    DateTime dt2  = dt0.AddSeconds(1800);
//
//    long diff = dt0 - dt;
//
//    int x = 3 ;

    Parameters parameters;

    std::vector<std::unique_ptr<Stem_flow_module> > modules;

    parameters.psi50_xylem = -2;
    parameters.psi88_xylem= -4;

    modules.push_back(std::make_unique<Linear_stem_flow>(parameters));
    modules.push_back(std::make_unique<Kirchhoff_Weibull_stem_flow>(parameters));
    modules.push_back(std::make_unique<Kirchhoff_Piecewise_Erf>(parameters));


    for (auto& module: modules) {
        module->Init();
    }

    double psi_leaf = -1;
    double psi_stem = -0.25;

    for (auto& module: modules) {
        std::cout << module->Get_Stem_flow(psi_stem, psi_leaf) << std::endl;
    }

    Swiss_Single_Test single_test;
    //Multi_Test single_test;
    //Hainich_Single_Test test;




}
