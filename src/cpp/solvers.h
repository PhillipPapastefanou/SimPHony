//
// Created by Phillip on 12.07.23.
//

#pragma once
#include <functional>

class Leaf_Stem_Implicit_Model;

class Bisection_solver_interface {

public:
    Bisection_solver_interface(Leaf_Stem_Implicit_Model& model, double prec, int max_steps);

    int Get_nsteps_converged();

    double Solve(double lower_bound, double upper_bound);

protected:
    virtual double f(double x) = 0;
    Leaf_Stem_Implicit_Model& model;

private:
    double prec;
    int max_steps;
    int steps_converged;
};

