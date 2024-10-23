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
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_Swiss_From_File(unittest.TestCase):
    def test_swiss_from_file(self):

        print("Calling the cpp lib directly from file...", end='')
        root_library_path = THIS_DIR
        root_data_path = os.path.join(root_library_path, os.pardir, os.pardir, 'data')
        # Specifying forcing and evalution data paths
        forcing_file = os.path.join(root_data_path, 'swiss', 'input', 'Forcing_Inter.csv')
        soil_water_file = os.path.join(root_data_path, 'swiss', 'input', 'vwc_swicc_cc_2023_indiv.csv')
        tree_path = os.path.join(root_data_path, 'swiss', 'eval', 'Trees')

        found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
        sys.path.append(cpp_lib_path)

        # Importing local libraries and paths
        from SimPHony import Simulation_Single_Swiss
        from SimPHony import DateTime

        parameter_file = os.path.join(THIS_DIR, 'test', 'input', "parameter_example_1.csv")

        create_example_swiss_cc_parameter_list(1, parameter_file)

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------

        # Set up the PHS simulation
        # read in the parameter file that we just created
        sim = Simulation_Single_Swiss()
        sim.Init_parameters_fn_single(parameter_file, 0)
        sim.Init_input(soil_water_file, forcing_file, tree_path)
        sim.Set_water_pot_initials(-1.0, -0.2)

        # Specify Start and End of the Simulation
        format = "%Y-%m-%d %H:%M:%S"
        date_start_str = "2018-05-01 00:00:00"
        date_end_str = "2018-12-15 00:00:00"
        timestart = DateTime(date_start_str, format)
        timeend = DateTime(date_end_str, format)

        # Run the simulation
        sim.Run(timestart, timeend)

        # Get output and analysis data
        # output = sim.Get_output()
        an = sim.Get_analysis()
        errors = an.Get_rmse()

        EPS = 8
        # REFERENCE_PSI_LEAF_0_RMSE = 1.8912050337701065;
        # REFERENCE_PSI_LEAF_6_RMSE = 2.5209254139403057;

        REFERENCE_PSI_LEAF_0_RMSE = 14.842429028013507;
        REFERENCE_PSI_LEAF_6_RMSE = 15.141204438155913;


        self.assertAlmostEqual(errors[0], REFERENCE_PSI_LEAF_0_RMSE, places=EPS)
        self.assertAlmostEqual(errors[6], REFERENCE_PSI_LEAF_6_RMSE, places=EPS)
        print_sucess("Done!")
