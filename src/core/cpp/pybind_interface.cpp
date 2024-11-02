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
#include "framework/parameters.h"
#include "modules/soil_layer.h"


namespace py = pybind11;
using std::vector;

PYBIND11_MODULE(SimPHony, handle){
//    py::class_<Simulation_Single_Hainich>(handle, "Simulation_Single_Hainich").
//            def("Init_parameters_def", &Simulation_Single_Hainich::Init_parameters_default).
//            def("Init_parameters_fn_single", &Simulation_Single_Hainich::Init_parameters_filename).
//            def("Init_parameters", &Simulation_Single_Hainich::Init_parameters).
//            def("Init_input", &Simulation_Single_Hainich::Init_input).
//            def("Set_water_pot_initials", &Simulation_Single_Hainich::Set_water_pot_initials).
//            def("Run", &Simulation_Single_Hainich::Run).
//            def("Get_output", &Simulation_Single_Hainich::Get_output).
//            def("Get_analysis", &Simulation_Single_Hainich::Get_analysis).
//            def(py::init<>());


    py::class_<Simulation_Single>(handle, "Simulation_Single").
            def("Read_config", &Simulation_Single::Read_Config).
            def("Init_parameters_def", &Simulation_Single::Init_parameters_default).
            def("Init_parameters_fn_single", &Simulation_Single::Init_parameters_filename).
            def("Init_parameters", &Simulation_Single::Init_parameters).
            def("Init_input", &Simulation_Single::Init_input).
            def("Set_water_pot_initials", &Simulation_Single::Set_water_pot_initials).
            def("Run", &Simulation_Single::Run).
            def("Get_output", &Simulation_Single::Get_output).
            def(py::init<>());

    py::class_<Simulation_Single_Swiss, Simulation_Single>(handle, "Simulation_Single_Swiss").
            def("Analyse", &Simulation_Single_Swiss::Analyse).
            def("Get_analysis", &Simulation_Single_Swiss::Get_analysis).
            def("Init_eval", &Simulation_Single_Swiss::Init_eval).
            def(py::init<>());

    py::class_<Simulation_Multi>(handle, "Simulation_Multi").
            def("Read_config", &Simulation_Multi::Read_Config).
            def("Init_Full_Parameter_Setups", &Simulation_Multi_Swiss::Init_Full_Parameter_Setups).
            def("Init_Partial_Parameter_Setups", &Simulation_Multi_Swiss::Init_Partial_Parameter_Setups).
            def("Init_input", &Simulation_Multi_Swiss::Init_input).
            def("Set_water_pot_initials", &Simulation_Multi_Swiss::Set_water_pot_initials).
            def("Run", &Simulation_Multi_Swiss::Run).
            def(py::init<int, bool>());

    py::class_<Simulation_Multi_Swiss, Simulation_Multi>(handle, "Simulation_Multi_Swiss").
            def("Init_eval", &Simulation_Multi_Swiss::Init_eval).
            def("Get_analysis_list", &Simulation_Multi_Swiss::Get_analysis_list).
            def(py::init<int, bool>());

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

            def("Get_J_per_area", &Output::Get_J_per_area).

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
            def(py::init<Model&, Swiss_Drought_Trees& , Parameters&>());

    py::class_<AnalysisHainich>(handle, "AnalysisHainich").
            def("Get_Rmse_G", &AnalysisHainich::Get_Rmse_G).
            def("Get_Rmse_J", &AnalysisHainich::Get_Rmse_J).
            def("Get_LL_G", &AnalysisHainich::Get_Log_Likelyhood_G).
            def("Get_LL_J", &AnalysisHainich::Get_Log_Likelyhood_J).
            def("Get_Rmse_psi_stem", &AnalysisHainich::Get_Rmse_psi_stem).
            def("Get_LL_psi_stem", &AnalysisHainich::Get_Log_Likelyhood_psi_stem).
            def(py::init<Model*, Parameters>());

    py::class_<TimeSlice>(handle, "TimeSlice").
            def_readwrite("Min", &TimeSlice::minimum).
            def_readwrite("Max", &TimeSlice::maximum).
            def_readwrite("Name", &TimeSlice::name).
            def_readwrite("Day_min", &TimeSlice::day_min).
            def(py::init<>());

    py::class_<DateTime>(handle, "DateTime").
            def(py::init<string, string>());

    py::enum_<Stem_flow_module_type>(handle, "Stem_flow_module_type").
            value("Linear", Stem_flow_module_type::Linear).
            value("KirchhoffPiecewiseErf", Stem_flow_module_type::KirchhoffPiecewiseErf).
            value("KirchhoffWeibull", Stem_flow_module_type::KirchhoffWeibull);

    py::enum_<Soil_water_module_type>(handle, "Soil_water_module_type").
            value("Campbell", Soil_water_module_type::Campbell).
            value("VanGenuchten", Soil_water_module_type::VanGenuchten).
            value("Saxton06", Soil_water_module_type::Saxton06);

    py::enum_<Conductivity_fraction_module_type>(handle, "Conductivity_fraction_module_type").
            value("Weibull", Conductivity_fraction_module_type::Weibull).
            value("Logit", Conductivity_fraction_module_type::Logit);

    py::class_<Soil_layer>(handle, "CSoil_layer").
            def_readwrite("organic_matter_fraction", &Soil_layer::organic_matter_fraction).
            def_readwrite("sand_fraction", &Soil_layer::sand_fraction).
            def_readwrite("clay_fraction", &Soil_layer::clay_fraction).

            def_readwrite("depth", &Soil_layer::depth).
            def_readwrite("k_soil_sat", &Soil_layer::k_soil_sat).
            def_readwrite("psi_soil_sat", &Soil_layer::psi_soil_sat).
            def_readwrite("theta_s", &Soil_layer::theta_s).
            def_readwrite("theta_r", &Soil_layer::theta_r).
            def_readwrite("pore_size_ind", &Soil_layer::pore_size_ind).
            def_readwrite("camp_b", &Soil_layer::camp_b).
            def(py::init<>());

    py::class_<Parameters>(handle, "CParameters").
            def_readwrite("id", &Parameters::id).

            def_readwrite("stem_flow_type", &Parameters::stem_flow_type).
            def_readwrite("soil_water_type", &Parameters::soil_water_type).
            def_readwrite("conductivity_fraction_type", &Parameters::conductivity_fraction_type).

            def_readwrite("root_area_index", &Parameters::root_area_index).
            def_readwrite("soil_profile_index", &Parameters::soil_profile_index).
            def_readwrite("canopy_height", &Parameters::canopy_height).
            def_readwrite("soil_layers", &Parameters::soil_layers).
            def_readwrite("stem_hydraulic_capacitance", &Parameters::stem_hydraulic_capacitance_max).
            def_readwrite("k_xylem_sat", &Parameters::k_xylem_sat).
            def_readwrite("huber_value", &Parameters::huber_value).
            def_readwrite("psi50_xylem", &Parameters::psi50_xylem).
            def_readwrite("psi88_xylem", &Parameters::psi88_xylem).
            def_readwrite("leaf_hydraulic_capacitance", &Parameters::leaf_hydraulic_capacitance).
            def_readwrite("leaf_area_index", &Parameters::leaf_area_index).
            def_readwrite("psi_leaf_50_close", &Parameters::psi_leaf_50_close).
            def_readwrite("d_50_close", &Parameters::d_50_close).
            def_readwrite("g0", &Parameters::g0).
            def_readwrite("g1", &Parameters::g1).
            def_readwrite("g_bark", &Parameters::g_bark).
            def_readwrite("jackson_root_beta", &Parameters::jackson_root_beta).
            def_readwrite("wcont_sigma_deviation", &Parameters::wcont_sigma_deviation).
            def_readwrite("dts_input", &Parameters::dts_input).
            def_readwrite("sw_rad_max", &Parameters::sw_rad_max).
            def_readwrite("anet_max", &Parameters::anet_max).
            def_readwrite("tree_density", &Parameters::tree_density).
            def_readwrite("n_stem_segments", &Parameters::n_stem_segments).
            def_readwrite("sustain_xylem_damage", &Parameters::sustain_xylem_damage).
            def_readwrite("permanent_xylem_fraction_threshold", &Parameters::permanent_xylem_fraction_threshold).
            def_readwrite("sigma_log_likelyhood", &Parameters::sigma_log_likelyhood).
            def_readwrite("max_psi_leaf_change_per_hour", &Parameters::max_psi_leaf_change_per_hour).
            def_readwrite("minimum_psi_leaf_multiplier", &Parameters::minimum_psi_leaf_multiplier).
            def_readwrite("solver_precision", &Parameters::solver_precision).
            def_readwrite("verbose", &Parameters::verbose).
            def_readwrite("dts", &Parameters::dts).
            def(py::init<>());

    handle.doc() = "PHS setup and running via python";
}
