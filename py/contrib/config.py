from enum import Enum
class Location(Enum):
    Invalid = -1
    Swiss = 0
    Hainich = 1

class Config:
    def __init__(self):
        self.build_path = '/Users/pp/Documents/Repos/hydro_standalone/cmake-build-release'
        self.lib_path  = '/Users/pp/Documents/Repos/plant_hydro_standalone/'
        self.theta_file  = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Water_Input_type2.csv"
        self.forcing_file  = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Forcing_Inter.csv"
        self.tree_folder_path  = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/IO/Trees"
        self.sap_file = ""
        self.output_path = ""
        self.location = Location.Invalid
        self.parameter_input_file_list  = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/Model/ParameterSampling/HypercubeSampling/Full_Parameter_setup.csv"
        self.parameter_input_file_list_partial  = "/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_experiment_simulation/Model/ParameterSampling/HypercubeSampling/Parial_Parameter_setup.csv"
