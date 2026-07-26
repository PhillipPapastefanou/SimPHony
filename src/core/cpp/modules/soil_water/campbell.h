//
// Created by Phillip on 03/09/2024.
//
#pragma once
#include <vector>
#include "soil_water_model.h"

class Campbell : public Soil_water_module {
public:
    Campbell(const Parameters &parameters, const Input &input, const Config& config);
    ~Campbell() override;

    void CalculatePsiAndKs(int start_idx = 0, int end_idx = -1) override;

    double Psi_from_theta(double theta, const Soil_layer& layer) const override;
    double K_from_theta(double theta, const Soil_layer& layer) const override;

};