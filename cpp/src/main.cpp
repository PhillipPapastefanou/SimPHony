#include "../Tests/swiss_single_test.h"
#include "../Tests/hainich_single_test.h"
#include "../Tests/multi_test.h"
#include "../Tests/soil_models_test.h"
#include "framework/date_time.h"
#include "../Tests/water_flow_segments_approx_test.h"
#include "auxil/rkf_solver.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

int main(int argc, char* argv[]) {

    // RKF_Solver solver;
    // solver.calc();

    // Water_flow_segmented_test water_flow_test;
    // Swiss_Single_Test single_test;

    Hainich_Single_Test test;
    std::cout << " aaaa  ";

//    Multi_Test multi_test;
//    std::cout << " bbbb  ";
}
