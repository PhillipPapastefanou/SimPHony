import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.param_generation.example_generator import create_example_swiss_cc_parameter_list
from src.contrib.auxil.messaging import print_failure , print_sucess
from src.contrib.config import Config, Location, Swiss_soil_water_input_type
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_Swiss_Multi_From_File(unittest.TestCase):
    def test_swiss_from_file(self):

        print("Calling the SimPHony Multi lib from file...", end='')
        root_library_path = THIS_DIR
        root_data_path = os.path.join(root_library_path, os.pardir, os.pardir, 'data')

        # Specifying forcing and evalution data paths
        config = Config()
        config.location = Location.Swiss_cc
        config.forcing_file = os.path.join(root_data_path, 'swiss', 'input', 'Forcing_Inter.csv')
        config.soilwater_file = os.path.join(root_data_path, 'swiss', 'input', 'vwc_swicc_cc_2023_indiv.csv')
        config.swiss_tree_folder_path = os.path.join(root_data_path, 'swiss', 'eval', 'Trees')
        config.parameters_list_file = os.path.join(THIS_DIR, 'test', 'input', "parameter_example_2.csv")
        config.swiss_soil_water_input_type = Swiss_soil_water_input_type.NLayers_Indiv
        config_path = os.path.join(THIS_DIR,'test', 'input', 'config_multi_py.txt')
        config.Export(config_path)

        found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
        sys.path.append(cpp_lib_path)

        # Importing local libraries and paths
        from SimPHony import Simulation_Multi_Swiss
        from SimPHony import DateTime

        create_example_swiss_cc_parameter_list(2, config.parameters_list_file)

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------

        rank = 0
        # Set up the PHS simulation
        # read in the parameter file that we just created
        sim = Simulation_Multi_Swiss(rank)
        sim.Read_config(config_path)
        sim.Init_Full_Parameter_Setups(np.arange(0, 2))
        sim.Init_input()
        sim.Set_water_pot_initials(-1.0, -0.2)

        # Specify Start and End of the Simulation
        format = "%Y-%m-%d %H:%M:%S"
        date_start_str = "2018-05-01 00:00:00"
        date_end_str = "2018-12-15 00:00:00"
        timestart = DateTime(date_start_str, format)
        timeend = DateTime(date_end_str, format)

        sim.Init_eval(timestart, timeend)
        # Run the simulation
        sim.Run(timestart, timeend)
        # Get output and analysis data
        # output = sim.Get_output()

        an = sim.Get_analysis_list()
        errors = an[1].Get_rmse()

        EPS = 8
        # REFERENCE_PSI_LEAF_0_RMSE = 1.8912050337701065;
        # REFERENCE_PSI_LEAF_6_RMSE = 2.5209254139403057;

        REFERENCE_PSI_LEAF_0_RMSE = 14.842429028013507;
        REFERENCE_PSI_LEAF_6_RMSE = 15.141204438155913;

        self.assertAlmostEqual(errors[0], REFERENCE_PSI_LEAF_0_RMSE, places=EPS)
        self.assertAlmostEqual(errors[6], REFERENCE_PSI_LEAF_6_RMSE, places=EPS)
        print_sucess("Done!")