//
// Created by Phillip on 22.04.24.
//

#include "lookup_table.h"
#include <iostream>
#include <cmath>


Lookup_table::Lookup_table() {

}

void Lookup_table::Init(std::vector<double> x, std::vector<double> y) {

    this->x = x;
    this->y = y;
    if (x.size() != y.size()){
        std::cout << "Lookup sizes do not match. Exiting...";
        exit(99);
    }

    xmin = *std::min_element(x.begin(), x.end());
    xmax = *std::max_element(x.begin(), x.end());
    size = x.size();

    m = (size - 1) / (xmax - xmin);
    b = -(size - 1) / (xmax - xmin) * xmin;
}

double Lookup_table::Get(double x) {
    if (x > xmax){
        return y.back();
    }
    if (x < xmin){
        return y.front();
    }
    int index = std::round(m * x + b);
    return y[index];
}


