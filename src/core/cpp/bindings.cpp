#include <emscripten/bind.h>
#include <vector>

// Include your actual headers here
#include "framework/parameters.h"
#include "io/input_hainich.h"
#include "io/output.h"
#include "framework/simulation_single.h"
#include "framework/simulation_single_hainich.h"

using namespace emscripten;

// Helper function to safely extract the derived input pointer
// This prevents you from having to modify your header files just for WebAssembly
Input_Hainich* Get_Input_Hainich_Helper(Simulation_Single_Hainich& sim) {
    // Assuming your base Simulation_Single class has `std::unique_ptr<Input> input;` 
    // accessible or a getter for it. (If `input` is protected, you might need a public getter in the base class).
    return static_cast<Input_Hainich*>(sim.Get_Input_Base_Pointer()); 
}

EMSCRIPTEN_BINDINGS(simphony_web_module) {
    
    // ==========================================
    // 1. STANDARD LIBRARY VECTORS
    // ==========================================
    // Register vectors so JavaScript can pass arrays back and forth
    register_vector<float>("VectorFloat");
    register_vector<long>("VectorLong");
    register_vector<int>("VectorInt");
    register_vector<std::vector<float>>("VectorVectorFloat"); // For 2D Theta array

    // ==========================================
    // 2. ENUMS
    // ==========================================
    enum_<Stem_flow_module_type>("Stem_flow_module_type")
        .value("Linear", Stem_flow_module_type::Linear)
        .value("KirchhoffWeibull", Stem_flow_module_type::KirchhoffWeibull)
        .value("KirchhoffPiecewiseErf", Stem_flow_module_type::KirchhoffPiecewiseErf);
        
    enum_<Conductivity_fraction_module_type>("Conductivity_fraction_module_type")
        .value("Weibull", Conductivity_fraction_module_type::Weibull)
        .value("Logit", Conductivity_fraction_module_type::Logit);

    enum_<Soil_water_module_type>("Soil_water_module_type")
        .value("Campbell", Soil_water_module_type::Campbell)
        .value("Saxton06", Soil_water_module_type::Saxton06)
        .value("VanGenuchten", Soil_water_module_type::VanGenuchten);

    // ==========================================
    // 3. PARAMETERS CLASS
    // ==========================================
    class_<Parameters>("Parameters")
        .constructor<>()
        .property("id", &Parameters::id)
        .property("root_area_index", &Parameters::root_area_index)
        .property("canopy_height", &Parameters::canopy_height)
        .property("stem_flow_type", &Parameters::stem_flow_type)
        .property("soil_water_type", &Parameters::soil_water_type)
        .property("psi_tlp", &Parameters::psi_tlp)
        .property("tree_density", &Parameters::tree_density)
        // Add any other properties you want to adjust from the web UI here
        ;

    // ==========================================
    // 4. INPUT & OUTPUT CLASSES
    // ==========================================
    class_<Input_Hainich>("Input_Hainich")
        // Allow JS to inject the data directly
        .function("Set_Forcing_Data", &Input_Hainich::Set_Forcing_Data)
        .function("Set_Forcing_Data_Fast", &Input_Hainich::Set_Forcing_Data_Fast);

    class_<Output>("Output")
        // Expose getters so JS can extract the results
        .function("Get_times", &Output::Get_times)
        .function("Get_T", &Output::Get_T)
        .function("Get_J", &Output::Get_J)
        .function("Get_psi_leaf", &Output::Get_psi_leaf)
        .function("Get_gs", &Output::Get_gs)

        .function("Get_T_view", &Output::Get_T_view)        
        .function("Get_J_view", &Output::Get_J_view)         
        .function("Get_psi_leaf_view", &Output::Get_psi_leaf_view)
        ;

    // ==========================================
    // 5. SIMULATION CLASSES (Base & Derived)
    // ==========================================
    class_<Simulation_Single>("Simulation_Single")
        // Base class handles parameter init, running, and getting output
        .function("Init_parameters", &Simulation_Single::Init_parameters)
        .function("Run", &Simulation_Single::Run)
        .function("Get_output", &Simulation_Single::Get_output);

    class_<Simulation_Single_Hainich, base<Simulation_Single>>("Simulation_Single_Hainich")
        .constructor<>()
        .function("Init_eval", &Simulation_Single_Hainich::Init_eval)
        .function("Analyse", &Simulation_Single_Hainich::Analyse)
        // Expose the input object so JS can feed it data (uses the helper function above)
        .function("Get_Input", &Get_Input_Hainich_Helper, allow_raw_pointers());
}