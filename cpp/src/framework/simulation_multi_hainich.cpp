//
// Created by Phillip on 16.03.24.
//

#include "simulation_multi_hainich.h"
#include "parameters.h"
#include "parameter_csv_reader.h"
#include "../modules/model.h"
#include <iostream>

Simulation_Multi_Hainich::Simulation_Multi_Hainich() {

}

void Simulation_Multi_Hainich::Init_input(std::string forcing_file, std::string sap_flow_file, int rank) {

    this->rank = rank;

    if(parameter_list.empty()){
        std::cout << "Parameter list is empty." << std::endl;
        std::cout << "This function needs to be called after reading the param list" << std::endl;
        exit(99);
    }

    const Parameters& params = std::get<0>(parameter_list.front());
    input = std::make_unique<Input_Hainich>(params);
    input->Add_Forcing_File(forcing_file);
    input->Read_N_Parse();

    sap_series = std::make_unique<TimeSeries>(sap_flow_file, true, ',');
    sap_series->Load("datetime", "%Y-%m-%d %H:%M:%S", {1});
}

void Simulation_Multi_Hainich::Init_Full_Parameter_Setups(string filename, std::vector<int> ids) {
    Parameter_CSV_Reader param_reader(filename);

    param_reader.Parse_Full_Files();

    vector<Parameters> all_parameters_list = param_reader.Get_parameter_list();

    for (int i = 0; i < ids.size(); ++i) {
        int index_of_interest = ids[i];
        std::tuple<Parameters, int> parameter_setup = std::make_tuple(all_parameters_list[index_of_interest], index_of_interest);
        parameter_list.push_back(parameter_setup);
    }
}

void Simulation_Multi_Hainich::Init_Partial_Parameter_Setups(string root_filename, string partial_parameter_filename,
                                                             std::vector<int> ids) {

}

void Simulation_Multi_Hainich::Set_water_pot_initials(double psi_leaf, double psi_stem) {
    init_psi_leaf = psi_leaf;
    init_psi_stem = psi_stem;
}

void Simulation_Multi_Hainich::Run(DateTime timestart, DateTime timeend) {

    std::cout << "Rank " << rank << ": Performing " << parameter_list.size() << " simulations." << std:: endl;

    auto start_simulatio = std::chrono::high_resolution_clock::now();
    auto start_timer = std::chrono::high_resolution_clock::now();

    if (parameter_list.size() == 0){
        std::cout << "No Parameter list specified. Skipping!" << std:: endl;
    }

    sap_series->GenerateModelObsIndexes(timestart, timeend, std::get<0>(parameter_list[0]).dts);

    for (int r = 0; r < parameter_list.size(); ++r) {

        Parameters& parameters = std::get<0>(parameter_list[r]);
        int parameter_index = std::get<1>(parameter_list[r]);

        Model model(parameters, *input);
        model.Set_derived_parameters();
        model.Set_initial_conditions(init_psi_leaf, init_psi_stem);
        model.Run(timestart,timeend);

        // Data analysis after simulation
        AnalysisHainich analysis(&model, parameters);
        analysis.CompareSapwood(*sap_series);
        analysis_list.push_back(analysis);


        auto end_timer = std::chrono::high_resolution_clock::now();
        auto elapsed_timer = std::chrono::duration_cast<std::chrono::milliseconds>( end_timer - start_timer);

        if (elapsed_timer.count() > 10000.0){

            auto elapsed_simulation = std::chrono::duration_cast<std::chrono::milliseconds>( end_timer - start_simulatio);

            std::cout << "Rank " << rank << ": Elapsed time: " << format_duration(elapsed_simulation) << " ";
            std::cout << "performed " << r << " out of " << parameter_list.size() << " simulations. "<< std::endl;
            start_timer = std::chrono::high_resolution_clock::now();
        }
    }

    std::cout << "Simulation finished! "<< std::endl;

}

std::vector<AnalysisHainich> Simulation_Multi_Hainich::Get_analysis_list() {
    return analysis_list;
}
