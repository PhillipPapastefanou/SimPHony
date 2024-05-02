//
// Created by Phillip on 12.07.23.
//

#include "solvers.h"
#include <iostream>


Bisection_solver_interface::Bisection_solver_interface(Solver_Indiv_Euler_Imp& model, double prec, int max_steps) :
model(model), prec(prec), max_steps(max_steps),n_errors(0) {}

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
            //std::cout << "Error: Solver did not converge between " << std::to_string(lower_bound) ;
            //std::cout << " and " << std::to_string(upper_bound) ;
            return false;
        }

        // We are converging against a value on the edge of the boundaries.
        if ((std::abs(s0 - upper_bound) < 1E-8) && (std::abs(s1 - upper_bound) < 1E-8)){
            return false;
        }

        if ((std::abs(s0 - lower_bound) < 1E-10) && (std::abs(s1 - lower_bound) < 1E-10)){
            return false;
        }

        // We do not have enough precision to calculate the correct water potential
        if(std::abs(s1-s0)< 1E-14){
            solution = s1;
            return true;
        }

    }

    solution = s2;
    steps_converged = step;
    return true;

}

int Bisection_solver_interface::Get_nsteps_converged() {
    return steps_converged;
}

double Bisection_solver_interface::Get_solution() {
    return solution;
}
