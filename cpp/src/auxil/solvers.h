//
// Created by Phillip on 12.07.23.
//

#pragma once
#include <functional>

class Leaf_Stem_Ground_Implicit_Model;

class Bisection_two_layer_solver_interface {

public:
    Bisection_two_layer_solver_interface(Leaf_Stem_Ground_Implicit_Model& model,
                                         double prec,
                                         int max_steps,
                                         double global_min,
                                         double global_max);
    int Get_nsteps_converged();
    double Solve(double lower_bound, double upper_bound, int solver_level = 1);

protected:
    virtual double f(double x) = 0;
    Leaf_Stem_Ground_Implicit_Model& model;

private:
    double prec;
    int max_steps;

    double global_min;
    double global_max;

    int steps_converged;
    const int nmax_errors = 2;
    int n_errors;
};

