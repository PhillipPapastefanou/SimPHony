//
// Created by Phillip on 14.07.23.
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

#include "simulation_single.h"
#include "simulation_multi.h"
#include "analysis.h"
#include "swiss_drought_trees.h"



namespace py = pybind11;
using std::vector;

PYBIND11_MODULE(hydro_standalone, handle){
    py::class_<Simulation_Single>(handle, "Simulation_Single").
            def("Init_parameters_def", &Simulation_Single::Init_parameters_default).
            def("Init_parameters_fn_single", &Simulation_Single::Init_parameters_filename).
            def("Init_input", &Simulation_Single::Init_input).
            def("Set_water_pot_initials", &Simulation_Single::Set_water_pot_initials).
            def("Run", &Simulation_Single::Run).
            def("Get_output", &Simulation_Single::Get_output).
            def("Get_analysis", &Simulation_Single::Get_analysis).
            def(py::init<>());

    py::class_<Simulation_Multi>(handle, "Simulation_Multi").
            def("Init_Full_Parameter_Setups", &Simulation_Multi::Init_Full_Parameter_Setups).
            def("Init_Partial_Parameter_Setups", &Simulation_Multi::Init_Partial_Parameter_Setups).
            def("Init_input", &Simulation_Multi::Init_input).
            def("Set_water_pot_initials", &Simulation_Multi::Set_water_pot_initials).
            def("Run", &Simulation_Multi::Run).
            def("Get_analysis_list", &Simulation_Multi::Get_analysis_list).
            def(py::init<>());

    py::class_<Output>(handle, "Output").
            def("Get_T", &Output::Get_T).
            def("Get_J", &Output::Get_J).
            def("Get_G", &Output::Get_G).
            def("Get_G_indiv", &Output::Get_G_indiv).

            def("Get_psi_leaf", &Output::Get_psi_leaf).
            def("Get_psi_stem", &Output::Get_psi_stem).
            def("Get_psi_soil_indiv", &Output::Get_psi_soil_indiv).

            def("Get_psi_soil_indiv", &Output::Get_psi_soil_indiv).
            def("Get_gs", &Output::Get_gs).
            def("Get_beta", &Output::Get_beta).

            def("Get_vpd", &Output::Get_vpd).
            def("Get_ks_soil", &Output::Get_ks_soil).

            def("Get_steps_psi_leaf", &Output::Get_steps_psi_leaf).
            def("Get_steps_psi_stem", &Output::Get_steps_psi_stem).

            def("Get_times", &Output::Get_times).
            def(py::init<>());


    py::class_<Analysis>(handle, "Analysis").
            def("Run", &Analysis::Run).
            def("Get_rmse", &Analysis::Get_rmse).
            def("Get_time_slices", &Analysis::Get_time_slices).
            def(py::init<Leaf_Stem_Implicit_Model*, Swiss_Drought_Trees >());

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
