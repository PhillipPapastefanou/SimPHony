//
// Created by Phillip on 16.03.24.
//
#include "simulation_single_hainich.h"
#include "parameter_csv_reader.h"
#include <iostream>


Simulation_Single_Hainich::Simulation_Single_Hainich(){

}

void Simulation_Single_Hainich::Init_input(std::string forcing_file,
                                           std::string sapflux_file,
                                           std::string psi_stem_file) {

    input = std::make_unique<Input_Hainich>();
    input->Add_Forcing_File(forcing_file);
    input->Read_N_Parse();

    sap_series = std::make_unique<TimeSeries>(sapflux_file, true, ',');
    sap_series->Load("datetime", "%Y-%m-%d %H:%M:%S", {1});


    psi_stem_series = std::make_unique<TimeSeries>(psi_stem_file, true, ',');
    psi_stem_series->Load("time", "%Y-%m-%d %H:%M:%S", {1});
}


void Simulation_Single_Hainich::Init_parameters(Parameters params) {
    parameters = std::make_unique<Parameters>(std::move(params));
}


void Simulation_Single_Hainich::Set_water_pot_initials(double psi_leaf, double psi_stem) {

    model = std::make_unique<Model>(*parameters, *input);
    model->Set_derived_parameters();
    model->Set_initial_conditions(psi_leaf, psi_stem);

}

void Simulation_Single_Hainich::Run(DateTime timestart, DateTime timeend) {

    sap_series->GenerateModelObsIndexes(timestart, timeend, parameters->dts);
    psi_stem_series->GenerateModelObsIndexes(timestart, timeend, parameters->dts);


    model->Run(timestart,timeend);
    analysis = std::make_unique<AnalysisHainich>(model.get(), *parameters);
    analysis->CompareSapwood(*sap_series);
    analysis->ComparePsiStem(*psi_stem_series);
}

Output Simulation_Single_Hainich::Get_output() {
    return model->Get_output();
}

void Simulation_Single_Hainich::Init_parameters_filename(std::string filename, int index) {

    Parameter_CSV_Reader param_reader(filename);

    param_reader.Parse_Full_Files();

    vector<Parameters> list = param_reader.Get_parameter_list();

    parameters = std::make_unique<Parameters>(list[index]);
}

AnalysisHainich Simulation_Single_Hainich::Get_analysis() {
    return *analysis;
}

void Simulation_Single_Hainich::Init_parameters_default() {
    parameters = std::make_unique<Parameters>();
}



