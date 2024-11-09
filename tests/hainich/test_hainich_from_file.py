import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir))

from src.contrib.auxil.messaging import print_failure ,print_sucess
from src.contrib.config import Config, Location
from src.contrib.auxil.setups import Setup
from src.contrib.param_generation.example_generator import create_example_hainich_parameter_list

class Test_Hainich_From_File(unittest.TestCase):
    def test_hainich_from_file(self):
        print("Exporting a parameter file and afterwards calling the lib...", end='')
        root_path = THIS_DIR
        scenario = "test"
        setup = Setup()
        setup.Apply_default_hainich()
        setup.Apply_default_paths(root_path, scenario)
        setup.config.parameters_list_file = os.path.join(setup.config.input_path, "parameters_example_1.csv")
        setup.config.config_file = os.path.join(setup.config.input_path, "config_py.txt")
        setup.Export()

        # Importing local libraries and paths
        sys.path.append(setup.config.build_folder)
        from SimPHony import Simulation_Single_Hainich
        from SimPHony import DateTime

        create_example_hainich_parameter_list(1, setup.config.parameters_list_file)

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------
        # Set up the PHS simulation
        # read in the parameter file that we just created
        sim = Simulation_Single_Hainich()
        sim.Read_config(setup.config.config_file)
        sim.Init_parameters_fn_single(0)
        sim.Init_input()
        sim.Set_water_pot_initials(-1.0, -0.2)

        # Specify Start and End of the Simulation
        format = "%Y-%m-%d %H:%M:%S"
        date_start_str = "2023-04-01 00:00:00"
        date_end_str = "2023-11-01 00:00:00"
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

        REFERENCE_J_RMSE = 0.002472019370519311;
        REFERENCE_PSI_STEM_RMSE = 0.3605217048395496;
        EPS = 8

        self.assertAlmostEqual(an.Get_Rmse_psi_stem(), REFERENCE_PSI_STEM_RMSE, places=EPS)
        self.assertAlmostEqual(an.Get_Rmse_J(), REFERENCE_J_RMSE, places=EPS)
        print_sucess("Done!")