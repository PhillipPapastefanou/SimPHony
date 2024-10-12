//
// Created by Phillip on 27.07.23.
//

#include "simulation_multi_swiss.h"
#include "parameter_csv_reader.h"
#include "../modules/model.h"
#include <iostream>
#include <chrono>

Simulation_Multi_Swiss::Simulation_Multi_Swiss() : rank(0) {

}

void Simulation_Multi_Swiss::Init_input(std::string theta_file, std::string forcing_file, std::string swiss_trees_folder, int rank) {

    this->rank = rank;
    this->theta_file = theta_file;
    this->forcing_file = forcing_file;

    if(parameter_list.empty()){
        std::cout << "Parameter list is empty." << std::endl;
        std::cout << "This function needs to be called after reading the param list" << std::endl;
        exit(99);
    }
    swiss_trees = std::make_unique<Swiss_Drought_Trees>(swiss_trees_folder);

    // Make this a variable input
    input = std::make_unique<Input_Swiss_Std_Variation>();
    input->Add_Soilwater_File(theta_file);
    input->Add_Forcing_File(forcing_file);
    input->Read_N_Parse();
}

void Simulation_Multi_Swiss::Init_Full_Parameter_Setups(string filename, std::vector<int> ids) {

    Parameter_CSV_Reader param_reader(filename);
    param_reader.Parse_Full_Files();
    vector<Parameters> all_parameters_list = param_reader.Get_parameter_list();

    for (int i = 0; i < ids.size(); ++i) {
        int index_of_interest = ids[i];
        std::tuple<Parameters, int> parameter_setup = std::make_tuple(all_parameters_list[index_of_interest], index_of_interest);
        parameter_list.push_back(parameter_setup);
    }
}

void Simulation_Multi_Swiss::Set_water_pot_initials(double psi_leaf, double psi_stem) {
    init_psi_leaf = psi_leaf;
    init_psi_stem = psi_stem;
}

void Simulation_Multi_Swiss::Run(DateTime timestart, DateTime timeend) {

    std::cout << "Rank " << rank << ": Performing " << parameter_list.size() << " simulations." << std:: endl;

    auto start_simulatio = std::chrono::high_resolution_clock::now();
    auto start_timer = std::chrono::high_resolution_clock::now();

    for (int r = 0; r < parameter_list.size(); ++r) {

        Parameters& params = std::get<0>(parameter_list[r]);

        // Parameter index is not yet being used
        int parameter_index = std::get<1>(parameter_list[r]);

        input = std::make_unique<Input_Swiss_Std_Variation>();
        input->Add_Soilwater_File(theta_file);
        input->Add_Forcing_File(forcing_file);
        input->Read_N_Parse();

        Model model(params, *input);
        model.Set_derived_parameters();
        model.Set_initial_conditions(init_psi_leaf, init_psi_stem);
        model.Run(timestart,timeend);

        // Data analysis after simulation
        Analysis_Swiss analysis(&model, *swiss_trees, params);
        analysis.Run();
        analysis_list.push_back(analysis);

        auto end_timer = std::chrono::high_resolution_clock::now();
        auto elapsed_timer = std::chrono::duration_cast<std::chrono::milliseconds>( end_timer - start_timer);

        if (elapsed_timer.count() > params.constants.TMUTE_MILLISEC){

            auto elapsed_simulation = std::chrono::duration_cast<std::chrono::milliseconds>( end_timer - start_simulatio);

            std::cout << "Rank " << rank << ": Elapsed time: " << format_duration(elapsed_simulation) << " ";
            std::cout << "performed " << r << " out of " << parameter_list.size() << " simulations. "<< std::endl;
            start_timer = std::chrono::high_resolution_clock::now();
        }
    }

    std::cout << "Simulation finished! "<< std::endl;
}

std::vector<Analysis_Swiss> Simulation_Multi_Swiss::Get_analysis_list() {
    return analysis_list;
}

void Simulation_Multi_Swiss::Init_Partial_Parameter_Setups(string root_filename, string partial_parameter_filename,
                                                           std::vector<int> ids) {

    Parameter_CSV_Reader main_param_file_reader(root_filename);
    main_param_file_reader.Parse_Full_Files();
    vector<Parameters> main_parameters_list = main_param_file_reader.Get_parameter_list();

    main_param_file_reader.Parse_Full_Files();

    if (main_parameters_list.empty()) {
        std::cout<< "No root parameter file in list" << std::endl;
        exit(-1);
    }

    //The main parameter list should contain only one file
    Parameters root_parameters = main_parameters_list.front();
    Parameter_CSV_Reader partial_parameter_reader(partial_parameter_filename);

    partial_parameter_reader.Parse_Partial_Files(root_parameters);

    vector<Parameters> all_parameters_list  = partial_parameter_reader.Get_parameter_list();

    for (int i = 0; i < ids.size(); ++i) {

        int index_of_interest = ids[i];

        std::tuple<Parameters, int> parameter_setup = std::make_tuple(all_parameters_list[index_of_interest], index_of_interest);

        parameter_list.push_back(parameter_setup);
    }

}

DateTime Simulation_Multi_Swiss::Get_first_year() {
    return input->dates.front();
}

DateTime Simulation_Multi_Swiss::Get_last_year() {
    return input->dates.back();
}

