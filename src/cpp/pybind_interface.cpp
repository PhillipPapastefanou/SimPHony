//
// Created by Phillip on 14.07.23.
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

#include "simulation.h"
#include "analysis.h"



namespace py = pybind11;
using std::vector;

PYBIND11_MODULE(hydro_standalone, handle){
    py::class_<Simulation>(handle, "Simulation").
            def("Init_parameters_def", &Simulation::Init_parameters_default).
            def("Init_parameters_fn_single", &Simulation::Init_parameters_filename).
            def("Init_input", &Simulation::Init_input).
            def("Set_water_pot_initials", &Simulation::Set_water_pot_initials).
            def("Run", &Simulation::Run).
            def("Get_output", &Simulation::Get_output).
            def("Get_analysis", &Simulation::Get_analysis).
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
            def(py::init<Leaf_Stem_Implicit_Model* >());

    py::class_<TimeSlice>(handle, "TimeSlice").
            def_readwrite("Min", &TimeSlice::minimum).
            def_readwrite("Max", &TimeSlice::maximum).
            def_readwrite("Name", &TimeSlice::name).
            def_readwrite("Day_min", &TimeSlice::day_min).
            def(py::init<>());

    handle.doc() = "PHS setup and running via python";
}
