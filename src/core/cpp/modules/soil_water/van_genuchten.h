//
// Created by Phillip on 03/09/2024.
//

#pragma once
#include <vector>
#include "soil_water_model.h"

class Van_Genuchten : public Soil_water_module {
public:
    Van_Genuchten(const Parameters &parameters, const Input &input, const Config &config);
    ~Van_Genuchten();

    void CalculatePsiAndKs(int start_idx = 0, int end_idx = -1) override;

    double Psi_from_theta(double theta, const Soil_layer& layer) const override;
    double K_from_theta(double theta, const Soil_layer& layer) const override;

private:


};