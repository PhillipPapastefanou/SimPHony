//
// Created by Phillip on 12.07.23.
//

#include "solvers.h"
#include <iostream>


Bisection_solver_interface::Bisection_solver_interface(Leaf_Stem_Implicit_Model& model, double prec, int max_steps) : model(model), prec(prec), max_steps(max_steps) {}

double Bisection_solver_interface::Solve(double lower_bound, double upper_bound) {

    double s0 = lower_bound;
    double s1 = upper_bound;


    // Actucally not neccessary to initialise
    double s2 = (s0 + s1) / 2.0;

    bool not_converged = true;
    int step = 0;

    while(not_converged){

        s2 = (s0 + s1) / 2.0;

        double y0 = f(s0);
        double y2 = f(s2);

        if(y0 * y2 < 0.0){
            s1 = s2;
        }
        else{
            s0 = s2;
        }

        step++;
        not_converged = std::abs(y2) > prec;

        if(step > max_steps){
            //Todo tell that something went wrong here
            std::cout << "Warning: Solver did not converge. " << std::endl;
            break;
        }


    }

    steps_converged = step;
    return s2;



}

int Bisection_solver_interface::Get_nsteps_converged() {
    return steps_converged;
}
