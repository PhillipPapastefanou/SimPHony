//
// Created by Phillip on 29.04.24.
//
#pragma once
#include "../framework/parameters.h"

class Condutivity_Module {

public:
    Condutivity_Module(const Parameters& params);

    virtual void Init() = 0;
    virtual double Get_fraction(double psi) = 0;
protected:
    double psi_50;
    double psi_88;
};


class WeibullCDF : public Condutivity_Module{
public:
    WeibullCDF(const Parameters& parameters);
    void Init() override;
    double Get_fraction(double psi) override;

private:
    double b;
    double c;
};


class Logit : public Condutivity_Module{
public:
    Logit(const Parameters& parameters);
    void Init() override;
    double Get_fraction(double psi) override;

private:
    double slope;
};

