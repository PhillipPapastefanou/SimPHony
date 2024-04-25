//
// Created by Phillip on 12.07.23.
//

#include "solvers.h"
#include <iostream>


Bisection_two_layer_solver_interface::Bisection_two_layer_solver_interface(Leaf_Stem_Ground_Implicit_Model& model, double prec, int max_steps,
                                                                           double global_min,
                                                                           double global_max) :
model(model), prec(prec), max_steps(max_steps),n_errors(0), global_min(global_min), global_max(global_max) {}

double Bisection_two_layer_solver_interface::Solve(double lower_bound, double upper_bound, int solver_level) {

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
            return lower_bound;
        }

        // We are converging against a value on the edge of the boundaries.
        if ((std::abs(s0 - upper_bound) < 1E-8) && (std::abs(s1 - upper_bound) < 1E-8)){
            if (solver_level == 1){
                std::cout << "Warning : Solver did not converge on first search space. Extending search space:" << std::endl;
                return Solve(global_min, global_max, 2);
            }
            else {
                std::cout << "Warning : Solver did not converge on second search space. Setting fixed water potential" << std::endl;
                return global_min;
            }
        }

        if ((std::abs(s0 - lower_bound) < 1E-10) && (std::abs(s1 - lower_bound) < 1E-10)){
            return lower_bound;
            //std::cout << "Warning : Solver did not converge on second search space. Setting fixed water potential" << std::endl;
        }

        // We do not have enough precision to calculate the correct water potential
        if(std::abs(s1-s0)< 1E-15){
            return s0;
        }

    }

    steps_converged = step;
    return s2;

}

int Bisection_two_layer_solver_interface::Get_nsteps_converged() {
    return steps_converged;
}
