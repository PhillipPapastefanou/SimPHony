import os
from src.contrib.config import Config, Location, Swiss_soil_water_input_type
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.auxil.files import get_forcing_filepath_swiss_cc
from src.contrib.auxil.files import get_soil_water_filepath_swiss_cc
from src.contrib.auxil.files import get_trees_psi_leaf_folder_path_cc

class Setup:

    def __init__(self):
        self.config = Config()

    def Apply_default_swiss(self,
                               soil_water_input_type: Swiss_soil_water_input_type = Swiss_soil_water_input_type.NLayers_Mean_N_Std):

        found_build, build_path, build_folder = get_SimPHony_build_path()
        if not found_build:
            print("Could not find build folder.")
            print("Check if SimPHony has been build already.")
            print("Exiting...")
            exit(99)
        self.config.location = Location.Swiss_cc
        self.config.build_path = build_path
        self.config.build_folder = build_folder
        self.config.forcing_file  = get_forcing_filepath_swiss_cc()
        self.config.soilwater_file  = get_soil_water_filepath_swiss_cc(soil_water_input_type)
        self.config.swiss_soil_water_input_type = soil_water_input_type
        self.config.swiss_tree_folder_path = get_trees_psi_leaf_folder_path_cc()

    def Apply_default_paths(self, root_output_directory, scenario_name):
        self.config.post_path = os.path.join(root_output_directory, scenario_name, 'post')
        self.config.output_path = os.path.join(root_output_directory, scenario_name, 'output')
        self.config.input_path = os.path.join(root_output_directory, scenario_name, 'input')
        self.config.scenario_path = os.path.join(root_output_directory, scenario_name)

    def Export(self,):
        self.config.Export(self.config.config_file)


