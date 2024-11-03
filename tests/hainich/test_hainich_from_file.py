import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

from src.contrib.auxil.messaging import print_sucess
from src.contrib.param_generation.example_generator import create_example_hainich_parameter_list
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.config import Config, Location, Swiss_soil_water_input_type
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_Hainich_From_File(unittest.TestCase):
    def test_hainich_from_file(self):
        print("Exporting a parameter file and afterwards calling the lib...", end='')
        root_library_path = THIS_DIR
        root_data_path = os.path.join(root_library_path, os.pardir, os.pardir, 'data')

        config = Config()
        config.location = Location.Hainich
        config.forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
        config.sap_flow_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
        config.psi_stem_file = os.path.join(root_data_path,'hainich', 'eval', 'stem_water_pot.csv')
        config.parameters_list_file = os.path.join(THIS_DIR, 'test', 'input',"parameter_example_1.csv")
        config_path = os.path.join(THIS_DIR,'test', 'input', 'config_py.txt')
        config.Export(config_path)

        found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
        sys.path.append(cpp_lib_path)

        # Importing local libraries and paths
        from SimPHony import Simulation_Single_Hainich
        from SimPHony import DateTime

        create_example_hainich_parameter_list(1, config.parameters_list_file)

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------
        # Set up the PHS simulation
        # read in the parameter file that we just created
        sim = Simulation_Single_Hainich()
        sim.Read_config(config_path)
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

        REFERENCE_J_RMSE = 0.002472322128072226;
        REFERENCE_PSI_STEM_RMSE = 0.3605197342451349;
        EPS = 8

        self.assertAlmostEqual(an.Get_Rmse_psi_stem(), REFERENCE_PSI_STEM_RMSE, places=EPS)
        self.assertAlmostEqual(an.Get_Rmse_J(), REFERENCE_J_RMSE, places=EPS)
        print_sucess("Done!")