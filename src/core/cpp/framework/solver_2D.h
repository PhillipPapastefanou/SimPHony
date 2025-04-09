//
// Created by Phillip on 25.04.24.
//

#pragma once
#include "water_potential_solver.h"
#include "../auxil/solvers.h"


class Solver_2D: public Water_Potential_Solver {

public:
    Solver_2D(const Parameters& params);
    void Init_solver() override;
    void Update_water_potentials(DateTime time) override;

protected:

    /// Derivative of the leaf water potential. Internal function [MPa s-1]
    double d_psi_leaf(double psi_leaf, double psi_stem);
    /// Derivative of the root water potential. Internal function.
    double d_psi_stem_ground(double psi_leaf, double psi_stem);

private:
    void balance_ponts(double xmin, double xmax, double ymin, double ymax , double delta);
};




