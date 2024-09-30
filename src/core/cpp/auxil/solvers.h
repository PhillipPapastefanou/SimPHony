//
// Created by Phillip on 12.07.23.
//

#pragma once
#include <functional>

class Solver_Indiv_Euler_Imp;

class Bisection_solver_interface {

public:
    Bisection_solver_interface(Solver_Indiv_Euler_Imp& model,
                               double prec,
                               int max_steps);

    int Get_nsteps_converged();
    double Solve(double lower_bound, double upper_bound);

    double Get_solution();

protected:
    virtual double f(double x) = 0;
    Solver_Indiv_Euler_Imp& model;


private:
    const int nmax_errors = 2;
    double prec;
    int max_steps;

    double solution;
    int steps_converged;
    int n_errors;
};

