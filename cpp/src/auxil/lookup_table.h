//
// Created by Phillip on 22.04.24.
//

#pragma once
#include <vector>
#include <algorithm>

class Lookup_table {

public:
    Lookup_table();
    void Init(std::vector<double> x, std::vector<double> y);
    double Get(double x);
private:
    std::vector<double> x;
    std::vector<double> y;

    double xmin;
    double xmax;

    double size;

    double m;
    double b;
};


