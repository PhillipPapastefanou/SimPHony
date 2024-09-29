//
// Created by Phillip on 25.04.24.
//

#pragma once
#include "../auxil/solvers.h"
#include "../framework/water_potential_solver.h"


class Bisection_psi_leaf;
class Bisection_psi_stem_ground;

class Solver_Indiv_Euler_Imp: public Water_Potential_Solver{

public:
    Solver_Indiv_Euler_Imp(const Parameters& params);
    void Init_solver() override;
    void Update_water_potentials() override;

    double psi_stem_root(double psi_stem_target);
    double psi_leaf_root(double psi_leaf_target);

private:

    void update_psi_leaf();
    void update_psi_stem_ground();

    bool calc_J_leaf;
    bool calc_J_stem;

    /// Main model solvers
    std::unique_ptr<Bisection_psi_leaf> solver_psi_leaf;
    std::unique_ptr<Bisection_psi_stem_ground> solver_psi_stem_ground;

    /// Derivative of the leaf water potential. Internal function [MPa s-1]
    double d_psi_leaf(double psi_leaf, double psi_stem);
    /// Derivative of the root water potential. Internal function.
    double d_psi_stem_ground(double psi_leaf, double psi_stem);


    double update_transpiration(double psi_leaf);

};


class Bisection_psi_leaf : public Bisection_solver_interface {

public:
    Bisection_psi_leaf(Solver_Indiv_Euler_Imp& model , double precision, int max_steps):
            Bisection_solver_interface(model, precision, max_steps){}
protected:
    double f(double x) override;
};

class Bisection_psi_stem_ground : public Bisection_solver_interface {

public:
    Bisection_psi_stem_ground(Solver_Indiv_Euler_Imp& model , double precision, int max_steps):
            Bisection_solver_interface(model, precision, max_steps){}
protected:
    double f(double x) override;
};





