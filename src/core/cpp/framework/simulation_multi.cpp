//
// Created by Phillip on 01/11/2024.
//

#include "simulation_multi.h"
#include "../io/input_swiss_mult_soils.h"
#include "parameter_csv_reader.h"
#include "../auxil/misc.h"

Simulation_Multi::Simulation_Multi(int rank, bool split_paramlist) :rank(rank), split_paramlist(split_paramlist){

}


void Simulation_Multi::Read_Config(std::string config_path) {
    config = std::make_unique<Config>();
    config->Read(config_path);
}

void Simulation_Multi::Init_input() {
    if (config == nullptr){
        std::cout << "Config file has not been specified. Did you forget to call the 'Read_Config' function?" << std::endl;
        std::cout << "Exiting...";
        exit(99);
    }

    input = std::make_unique<Input_Swiss_Multi_Soils>(*config);
    input->Read_N_Parse();
}

void Simulation_Multi::Init_Full_Parameter_Setups(std::vector<int> ids) {

    if (config == nullptr){
        std::cout << "Config file has not been specified. Did you forget to call the 'Read_Config' function?" << std::endl;
        std::cout << "Exiting...";
        exit(99);
    }

    std::unique_ptr<Parameter_CSV_Reader> param_reader;

    if (split_paramlist){
        param_reader = std::make_unique<Parameter_CSV_Reader>(config->parameters_list_file.Get() + std::to_string(rank));
    }
    else{
        param_reader = std::make_unique<Parameter_CSV_Reader>(config->parameters_list_file.Get());
    }

    param_reader->Parse_Full_Files();
    vector<Parameters> all_parameters_list = param_reader->Get_parameter_list();

    if (all_parameters_list.size() < ids.size()){
        std::cout << "Parameter list has less entry than required ids!" << std::endl;
        std::cout << "Exiting...";
        exit(99);
    }

    for (int i = 0; i < ids.size(); ++i) {
        int index_of_interest = ids[i];
        std::tuple<Parameters, int> parameter_setup = std::make_tuple(all_parameters_list[index_of_interest], index_of_interest);
        parameter_list.push_back(parameter_setup);
    }
}

void Simulation_Multi::Init_Partial_Parameter_Setups(std::vector<int> ids) {

    Parameter_CSV_Reader main_param_file_reader(config->parameters_list_file.Get());
    main_param_file_reader.Parse_Full_Files();
    vector<Parameters> main_parameters_list = main_param_file_reader.Get_parameter_list();

    main_param_file_reader.Parse_Full_Files();

    if (main_parameters_list.empty()) {
        std::cout<< "No root parameter file in list" << std::endl;
        exit(-1);
    }

    std::cout << "Currently not implemented" << std::endl;
    exit(99);

    //The main parameter list should contain only one file
    Parameters root_parameters = main_parameters_list.front();
    Parameter_CSV_Reader partial_parameter_reader("to_be_renamed");

    partial_parameter_reader.Parse_Partial_Files(root_parameters);

    vector<Parameters> all_parameters_list  = partial_parameter_reader.Get_parameter_list();

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

void Simulation_Multi::Run(DateTime timestart, DateTime timeend) {

    std::cout << "Rank " << rank << ": Performing " << parameter_list.size() << " simulations." << std:: endl;

    auto start_simulation = std::chrono::high_resolution_clock::now();
    auto start_timer = std::chrono::high_resolution_clock::now();

    for (int r = 0; r < parameter_list.size(); ++r) {

        Parameters& params = std::get<0>(parameter_list[r]);
        int parameter_index = std::get<1>(parameter_list[r]);

        Model model(params, *input, *config);
        model.Set_derived_parameters();
        model.Set_initial_conditions(init_psi_leaf, init_psi_stem);
        model.Run(timestart,timeend);

        Update_Analysis(model, params);

        // Data analysis after simulation
//        Analysis_Swiss analysis(&model, *swiss_trees, params);
//        analysis.Run();
//        analysis_list.push_back(analysis);
//
//        // Data analysis after simulation
//        AnalysisHainich analysis(&model, parameters);
//        analysis.CompareSapwood(*sap_series);
//        analysis.ComparePsiStem(*psi_stem_series);
//        analysis_list.push_back(analysis);

        auto end_timer = std::chrono::high_resolution_clock::now();
        auto elapsed_timer = std::chrono::duration_cast<std::chrono::milliseconds>( end_timer - start_timer);

        if (elapsed_timer.count() > params.constants.TMUTE_MILLISEC){
            auto elapsed_simulation = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_simulation);
            std::cout << "Rank " << rank << " completed " << r << " out of " << parameter_list.size() << " runs. ";
            std::cout << "Elapsed time: " << format_duration(elapsed_simulation) << " remaining: "
                      << remaining_str(elapsed_simulation, r, parameter_list.size())  << "." << std::endl;
            start_timer = std::chrono::high_resolution_clock::now();
        }

        // Because of the pybind module output of each model when associated with the analysis might not be cleared automatically
        model.Clear_output();

    }

    std::cout << "Simulation finished! "<< std::endl;

}

void Simulation_Multi::Init_eval(DateTime timestart, DateTime timeend) {

}

void Simulation_Multi::Update_Analysis(const Model &model, const Parameters &parameters) {
    std::cout << "Warning: No analysis perfomed" << std::endl;
}

