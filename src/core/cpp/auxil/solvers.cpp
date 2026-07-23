//
// Created by Phillip on 12.07.23.
//

#include "solvers.h"
#include <iostream>


Bisection_solver_interface::Bisection_solver_interface(Solver_Indiv_Euler_Imp& model, double prec, int max_steps) :
model(model), prec(prec), max_steps(max_steps),n_errors(0) {}

double Bisection_solver_interface::Solve(double lower_bound, double upper_bound) {

    // TEMP A/B test: regula falsi (Illinois variant) in place of plain
    // bisection. Instead of always trying the bracket midpoint, it draws a
    // secant line through the two bracket endpoints and uses where that
    // line crosses zero as the next trial point -- converges superlinearly
    // for smooth f (vs bisection's fixed linear rate), while staying inside
    // the same bracket-and-sign-check safety net. y0/y1 are cached and only
    // the endpoint that actually moved gets re-evaluated each iteration
    // (bisection's own loop recomputed f(s0) unconditionally every time,
    // even on iterations where s0 didn't change). The "Illinois" half-step
    // (halving the stale endpoint's value when the same side is retained
    // twice in a row) is the standard fix for plain regula falsi's
    // known stagnation failure mode on asymmetric functions.
    double s0 = lower_bound;
    double s1 = upper_bound;
    double y0 = f(s0);
    double y1 = f(s1);

    double s2 = s0;
    double y2 = y0;

    bool not_converged = true;
    int step = 0;
    int stale_side = -1; // 0: s0 stale, 1: s1 stale, -1: neither yet

    while(not_converged){

        if (std::abs(y1 - y0) < 1E-300) {
            // Degenerate secant (shouldn't happen for a valid bracket) --
            // fall back to a plain bisection step rather than divide by ~0.
            s2 = (s0 + s1) / 2.0;
        } else {
            s2 = (s0 * y1 - s1 * y0) / (y1 - y0);
        }
        y2 = f(s2);

        if(y0 * y2 < 0.0){
            s1 = s2;
            y1 = y2;
            if (stale_side == 0) y0 *= 0.5;
            stale_side = 0;
        }
        else{
            s0 = s2;
            y0 = y2;
            if (stale_side == 1) y1 *= 0.5;
            stale_side = 1;
        }

        step++;
        not_converged = std::abs(y2) > prec;

        if(step > max_steps){
            //std::cout << "Error: Solver did not converge between " << std::to_string(lower_bound) ;
            //std::cout << " and " << std::to_string(upper_bound) ;
            return false;
        }

        // We are converging against a value on the edge of the boundaries.
        if ((std::abs(s0 - upper_bound) < 1E-14) && (std::abs(s1 - upper_bound) < 1E-14)){
            return false;
        }

        if ((std::abs(s0 - lower_bound) < 1E-14) && (std::abs(s1 - lower_bound) < 1E-14)){
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
