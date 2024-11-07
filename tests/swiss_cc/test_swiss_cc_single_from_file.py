import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir))

from src.contrib.auxil.messaging import print_sucess
from src.contrib.auxil.setups import Setup
from src.contrib.config import Config, Swiss_soil_water_input_type
from src.contrib.param_generation.example_generator import create_example_swiss_cc_parameter_list

class Test_Swiss_Single_From_File(unittest.TestCase):
    def test_swiss_from_file(self):

        print("Calling the cpp lib directly from file...", end='')
        root_path = THIS_DIR
        scenario = "test"

        setup = Setup()
        setup.Apply_default_swiss(Swiss_soil_water_input_type.NLayers_Mean_N_Std)
        setup.Apply_default_paths(root_path, scenario)
        setup.config.parameters_list_file = os.path.join(setup.config.input_path, "parameters_example_1.csv")
        setup.config.config_file = os.path.join(setup.config.input_path, "config_py.txt")
        setup.Export()

        # Specifying forcing and evalution data paths
        sys.path.append(setup.config.build_folder)
        from SimPHony import Simulation_Single_Swiss
        from SimPHony import DateTime

        create_example_swiss_cc_parameter_list(1, setup.config.parameters_list_file)

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------

        # Set up the PHS simulation
        # read in the parameter file that we just created
        sim = Simulation_Single_Swiss()
        sim.Read_config(setup.config.config_file)
        sim.Init_parameters_fn_single(0)
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
        # Perform the analysis
        sim.Analyse()

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