//
// Created by Phillip on 14.07.23.
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

#include "framework/simulation_single_swiss.h"
#include "framework/simulation_single_hainich.h"
#include "framework/simulation_multi_swiss.h"
#include "framework/simulation_multi_hainich.h"
#include "io/analysis_swiss.h"
#include "io/analysis_hainich.h"
#include "framework/parameters.h"
#include "io/swiss_drought_trees.h"



namespace py = pybind11;
using std::vector;

PYBIND11_MODULE(hydro_standalone, handle){
    py::class_<Simulation_Single_Hainich>(handle, "Simulation_Single_Hainich").
            def("Init_parameters_def", &Simulation_Single_Hainich::Init_parameters_default).
            def("Init_parameters_fn_single", &Simulation_Single_Hainich::Init_parameters_filename).
            def("Init_input", &Simulation_Single_Hainich::Init_input).
            def("Set_water_pot_initials", &Simulation_Single_Hainich::Set_water_pot_initials).
            def("Run", &Simulation_Single_Hainich::Run).
            def("Get_output", &Simulation_Single_Hainich::Get_output).
            def("Get_analysis", &Simulation_Single_Hainich::Get_analysis).
            def(py::init<>());

    py::class_<Simulation_Single_Swiss>(handle, "Simulation_Single_Swiss").
            def("Init_parameters_def", &Simulation_Single_Swiss::Init_parameters_default).
            def("Init_parameters_fn_single", &Simulation_Single_Swiss::Init_parameters_filename).
            def("Init_input", &Simulation_Single_Swiss::Init_input).
            def("Set_water_pot_initials", &Simulation_Single_Swiss::Set_water_pot_initials).
            def("Run", &Simulation_Single_Swiss::Run).
            def("Get_output", &Simulation_Single_Swiss::Get_output).
            def("Get_analysis", &Simulation_Single_Swiss::Get_analysis).
            def(py::init<>());

    py::class_<Simulation_Multi_Swiss>(handle, "Simulation_Multi_Swiss").
            def("Init_Full_Parameter_Setups", &Simulation_Multi_Swiss::Init_Full_Parameter_Setups).
            def("Init_Partial_Parameter_Setups", &Simulation_Multi_Swiss::Init_Partial_Parameter_Setups).
            def("Init_input", &Simulation_Multi_Swiss::Init_input).
            def("Set_water_pot_initials", &Simulation_Multi_Swiss::Set_water_pot_initials).
            def("Run", &Simulation_Multi_Swiss::Run).
            def("Get_analysis_list", &Simulation_Multi_Swiss::Get_analysis_list).
            def(py::init<>());

    py::class_<Simulation_Multi_Hainich>(handle, "Simulation_Multi_Hainich").
            def("Init_Full_Parameter_Setups", &Simulation_Multi_Hainich::Init_Full_Parameter_Setups).
            def("Init_Partial_Parameter_Setups", &Simulation_Multi_Hainich::Init_Partial_Parameter_Setups).
            def("Init_input", &Simulation_Multi_Hainich::Init_input).
            def("Set_water_pot_initials", &Simulation_Multi_Hainich::Set_water_pot_initials).
            def("Run", &Simulation_Multi_Hainich::Run).
            def("Get_analysis_list", &Simulation_Multi_Hainich::Get_analysis_list).
            def(py::init<>());

    py::class_<Output>(handle, "Output").
            def("Get_T", &Output::Get_T).
            def("Get_J", &Output::Get_J).
            def("Get_G", &Output::Get_G).
            def("Get_G_indiv", &Output::Get_G_indiv).

            def("Get_G_per_sap", &Output::Get_G_per_sap).
            def("Get_J_per_sap", &Output::Get_J_per_sap).

            def("Get_psi_leaf", &Output::Get_psi_leaf).
            def("Get_psi_stem", &Output::Get_psi_stem).
            def("Get_psi_soil_indiv", &Output::Get_psi_soil_indiv).

            def("Get_gs", &Output::Get_gs).
            def("Get_beta", &Output::Get_beta).

            def("Get_vpd", &Output::Get_vpd).
            def("Get_anet", &Output::Get_anet).
            def("Get_ks_soil", &Output::Get_ks_soil).

            def("Get_steps_psi_leaf", &Output::Get_steps_psi_leaf).
            def("Get_steps_psi_stem", &Output::Get_steps_psi_stem).

            def("Get_times", &Output::Get_times).
            def(py::init<const Parameters&>());


    py::class_<Analysis_Swiss>(handle, "Analysis_Swiss").
            def("Run", &Analysis_Swiss::Run).
            def("Get_rmse", &Analysis_Swiss::Get_rmse).
            def("Get_time_slices", &Analysis_Swiss::Get_time_slices).
            def(py::init<Leaf_Stem_Ground_Implicit_Model*, Swiss_Drought_Trees >());

    py::class_<AnalysisHainich>(handle, "AnalysisHainich").
            def("Get_Rmse_G", &AnalysisHainich::Get_Rmse_G).
            def("Get_Rmse_J", &AnalysisHainich::Get_Rmse_J).
            def("Get_LL_G", &AnalysisHainich::Get_Log_Likelyhood_G).
            def("Get_LL_J", &AnalysisHainich::Get_Log_Likelyhood_J).
            def(py::init<Leaf_Stem_Ground_Implicit_Model*, Parameters>());

    py::class_<TimeSlice>(handle, "TimeSlice").
            def_readwrite("Min", &TimeSlice::minimum).
            def_readwrite("Max", &TimeSlice::maximum).
            def_readwrite("Name", &TimeSlice::name).
            def_readwrite("Day_min", &TimeSlice::day_min).
            def(py::init<>());

    py::class_<DateTime>(handle, "DateTime").
            def(py::init<string, string>());

    handle.doc() = "PHS setup and running via python";
}
