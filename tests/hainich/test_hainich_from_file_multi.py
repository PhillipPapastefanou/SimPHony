import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir))

from src.contrib.param_generation.example_generator import create_example_hainich_parameter_list
from src.contrib.auxil.messaging import print_sucess
from src.contrib.auxil.setups import Setup
from src.contrib.config import Config, Location, Swiss_soil_water_input_type

class Test_Hainich_From_File_Multi(unittest.TestCase):
    def test_hainich_from_file_multi(self):
        print("Multiple: Calling the lib multiple times...", end='')
        root_path = THIS_DIR
        scenario = "test"

        setup = Setup()
        setup.Apply_default_hainich()
        setup.Apply_default_paths(root_path, scenario)
        setup.config.parameters_list_file = os.path.join(setup.config.input_path, "parameters_example_2.csv")
        setup.config.config_file = os.path.join(setup.config.input_path, "config_multi_py.txt")
        setup.Export()

        # Specifying forcing and evalution data paths
        sys.path.append(setup.config.build_folder)

        # Importing local libraries and paths
        from SimPHony import Simulation_Multi_Hainich
        from SimPHony import DateTime

        create_example_hainich_parameter_list(2, setup.config.parameters_list_file)

        os.makedirs(os.path.join(THIS_DIR, 'test', 'input'), exist_ok=True)
        parameter_file = os.path.join(THIS_DIR, 'test', 'input', "parameter_example_2.csv")

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------
        # Set up the PHS simulation
        # read in the parameter file that we just created
        rank = 0
        sim = Simulation_Multi_Hainich(rank, False)
        sim.Read_config(setup.config.config_file)
        sim.Init_Full_Parameter_Setups(np.arange(0, 2))
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

        REFERENCE_J_RMSE = [0.0018529536133690052, 0.0012463240127134915]
        REFERENCE_PSI_STEM_RMSE = [0.22891311164441566, 0.30494198664184896]
        EPS = 8

        # Get analysis data
        an_list = sim.Get_analysis_list()

        # Compare to setup
        for i in range(len(an_list)):
            self.assertAlmostEqual(an_list[i].Get_Rmse_psi_stem(), REFERENCE_PSI_STEM_RMSE[i], places=EPS)
            self.assertAlmostEqual(an_list[i].Get_Rmse_J(), REFERENCE_J_RMSE[i], places=EPS)
        print_sucess("Done!")