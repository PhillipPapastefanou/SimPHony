//
// Created by Phillip on 03/09/2024.
//
#pragma once
#include <vector>
#include "soil_water_model.h"

class Campbell : public Soil_water_module {
public:
    Campbell(const Parameters &parameters, const Input &input);
    ~Campbell();

    void CalculatePsiAndKs() override;

};