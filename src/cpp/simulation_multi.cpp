//
// Created by Phillip on 27.07.23.
//

#include "simulation_multi.h"
#include "parameter_csv_reader.h"
#include "model.h"
#include <iostream>
#include <chrono>

Simulation_Multi::Simulation_Multi() : rank(0) {



}

void Simulation_Multi::Init_input(std::string theta_file, std::string forcing_file, std::string swiss_trees_folder, int rank) {

    this->rank = rank;
    input = std::make_unique<Input>(theta_file, forcing_file);
    input->Read_N_Parse();

    swiss_trees = std::make_unique<Swiss_Drought_Trees>(swiss_trees_folder);

}

void Simulation_Multi::Init_Full_Parameter_Setups(string filename, std::vector<int> ids) {

    Parameter_CSV_Reader param_reader(filename);

    param_reader.Parse_Full_Files();

    vector<Parameters> all_parameters_list = param_reader.Get_parameter_list();

    for (int i = 0; i < ids.size(); ++i) {

        int index_of_interest = ids[i];

        std::tuple<Parameters, int> parameter_setup = std::make_tuple(all_parameters_list[index_of_interest], index_of_interest);

        parameter_list.push_back(parameter_setup);
    }


}

void Simulation_Multi::Set_water_pot_initials(double psi_leaf, double psi_stem) {
    init_psi_leaf = psi_leaf;
    init_psi_stem = psi_stem;
}

void Simulation_Multi::Run(double steplen, double timestart, double timeend) {

    std::cout << "Rank " << rank << ": Performing " << parameter_list.size() << " simulations." << std:: endl;

    auto start_simulatio = std::chrono::high_resolution_clock::now();
    auto start_timer = std::chrono::high_resolution_clock::now();

    for (int r = 0; r < parameter_list.size(); ++r) {

        Parameters& parameters = std::get<0>(parameter_list[r]);
        int parameter_index = std::get<1>(parameter_list[r]);

        Leaf_Stem_Implicit_Model model(parameters, *input);
        model.Set_derived_parameters();
        model.Set_initial_conditions(init_psi_leaf, init_psi_stem);
        model.Run(steplen,timestart,timeend);

        // Data analysis after simulation
        Analysis analysis(&model, *swiss_trees);

        analysis.Run();
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

std::vector<Analysis> Simulation_Multi::Get_analysis_list() {
    return analysis_list;
}

void Simulation_Multi::Init_Partial_Parameter_Setups(string root_filename, string partial_parameter_filename,
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

