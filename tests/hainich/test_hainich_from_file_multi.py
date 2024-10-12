import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime
from src.contrib.param_generation.example_generator import create_example_parameter_list
from src.contrib.auxil.files import get_SimPHony_build_path
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_Hainich_From_File_Multi(unittest.TestCase):
    def test_hainich_from_file_multi(self):
        print("Multiple: Exporting a parameter file and afterwards calling the lib...", end='')
        root_library_path = THIS_DIR
        root_data_path = os.path.join(root_library_path, os.pardir, os.pardir, 'data')
        # Specifying forcing and evalution data paths
        forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
        sapflux_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
        psi_stem_file = os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')

        found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
        sys.path.append(cpp_lib_path)

        # Importing local libraries and paths
        from SimPHony import Simulation_Multi_Hainich
        from SimPHony import DateTime

        os.makedirs(os.path.join(THIS_DIR, 'test', 'input'), exist_ok=True)
        parameter_file = os.path.join(THIS_DIR, 'test', 'input', "parameter_example_2.csv")
        create_example_parameter_list(2, parameter_file)

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------
        # Set up the PHS simulation
        # read in the parameter file that we just created
        sim = Simulation_Multi_Hainich()
        sim.Init_Full_Parameter_Setups(parameter_file, np.arange(0, 2))
        sim.Init_input(forcing_file, sapflux_file, psi_stem_file, 0)
        sim.Set_water_pot_initials(-1.0, -0.2)

        # Specify Start and End of the Simulation
        format = "%Y-%m-%d %H:%M:%S"
        date_start_str = "2023-04-01 00:00:00"
        date_end_str = "2023-11-01 00:00:00"
        timestart = DateTime(date_start_str, format)
        timeend = DateTime(date_end_str, format)

        # Run the simulation
        sim.Run(timestart, timeend)

        REFERENCE_J_RMSE = [0.002554028773026848, 0.000693889552292396]
        REFERENCE_PSI_STEM_RMSE = [0.7289339483826249, 1.9520272480037195]
        EPS = 8

        # Get analysis data
        an_list = sim.Get_analysis_list()

        # Compare to setup
        for i in range(len(an_list)):
            self.assertAlmostEqual(an_list[i].Get_Rmse_psi_stem(), REFERENCE_PSI_STEM_RMSE[i], places=EPS)
            self.assertAlmostEqual(an_list[i].Get_Rmse_J(), REFERENCE_J_RMSE[i], places=EPS)
        print("Done!")
