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
from src.contrib.config import Config, Location, Swiss_soil_water_input_type
from src.contrib.auxil.setups import Setup

class Test_Swiss_Single_Direct(unittest.TestCase):
    def test_swiss_direct(self):

        print("Calling the cpp lib directly from python...", end='')

        root_path = THIS_DIR
        scenario = "test"

        setup = Setup()
        setup.Apply_default_swiss(Swiss_soil_water_input_type.NLayers_Indiv)
        setup.Apply_default_paths(root_path, scenario)
        #setup.config.parameters_list_file = os.path.join(setup.config.post_path, "parameters_best_mean_alive.csv")
        setup.config.config_file = os.path.join(setup.config.input_path, "config_py.txt")
        setup.Export()

        # Specifying forcing and evalution data paths
        sys.path.append(setup.config.build_folder)
        from SimPHony import Simulation_Single_Swiss
        from SimPHony import DateTime

        from src.core.py.Parameters import Parameters
        from src.core.py.Parameters import SoilLayer
        from src.core.py.Parameters import Soil_Water_Model_Type
        from src.core.py.Parameters import Convert_Soil_Parameters

        # Create parameter setup
        params = Parameters()

        nsoil_layers = 3
        layer = SoilLayer()
        layer.k_soil_sat = 1.0 / 100.0 / 86400.0
        layer.psi_soil_sat = -0.5 * 1
        # layer.camp_b  = 10.4
        layer.theta_s = 0.48
        layer.theta_r = 0.05
        layer.pore_size_ind = 0.6

        # Copy the soil layer and assume all layers have the same properties...
        soil_layers = []
        for s in range(nsoil_layers):
            soil_layers.append(copy.deepcopy(layer))

        # ... but not the depth
        soil_layers[0].depth = 0.1
        soil_layers[1].depth = 0.3
        soil_layers[2].depth = 0.4

        # ------------------------------------------------------
        # Parameter setup
        # ------------------------------------------------------

        Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

        params.canopy_height = 31
        params.huber_value = 1.0 / 3000.0
        params.k_xylem_sat = 5 * 1000 / 18
        params.stem_hydraulic_capacitance = 150 * 1000 / 18
        params.leaf_hydraulic_capacitance = 0.01 * 1000 / 18
        params.g_bark = 0.01
        params.g0 = 0.005
        params.g1 = 1.5
        params.leaf_area_index = 4.8
        params.psi_leaf_50_close = -2.3
        params.d_50_close = 2.0
        params.psi50_xylem = -3.5;
        params.psi88_xylem = -5.5;
        params.root_area_index = 4.5
        params.jackson_root_beta = 0.96
        params.tree_density = 64.0 / 10000.0
        params.anet_max = 2.5
        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.sw_rad_max = 1040

        cparameters = params.Create_CParameters(soil_layers=soil_layers)

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------
        # Set up the PHS simulation
        # read in the parameter file that we just created
        sim = Simulation_Single_Swiss()
        sim.Read_config(setup.config.config_file)
        sim.Init_parameters(cparameters)
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
        # Previous values for the other soil water input
        # REFERENCE_PSI_LEAF_0_RMSE = 1.8912050337701065;
        # REFERENCE_PSI_LEAF_6_RMSE = 2.5209254139403057;

        REFERENCE_PSI_LEAF_0_RMSE = 2.061104366482163;
        REFERENCE_PSI_LEAF_6_RMSE = 2.339620165100023;

        self.assertAlmostEqual(errors[0], REFERENCE_PSI_LEAF_0_RMSE, places=EPS)
        self.assertAlmostEqual(errors[6], REFERENCE_PSI_LEAF_6_RMSE, places=EPS)
        print_sucess("Done!")

    def test_swiss_direct_multi(self):

        print("Calling the cpp lib directly from python...", end='')

        from src.contrib.swiss_tree_parser import SwissTreeParser
        from src.contrib.auxil.setups import Setup
        from src.contrib.auxil.output_df import create_output_df
        from src.contrib.auxil.files import get_trees_psi_leaf_folder_path_cc
        from src.contrib.config import Config, Swiss_soil_water_input_type

        root_path = THIS_DIR
        scenario = "test"

        setup = Setup()
        setup.Apply_default_swiss(Swiss_soil_water_input_type.NLayers_Indiv)
        setup.Apply_default_paths(root_path, scenario)
        setup.config.parameters_list_file = os.path.join(setup.config.post_path, "parameters_best_mean_alive.csv")
        setup.config.config_file = os.path.join(setup.config.input_path, "config_dummy.txt")
        setup.Export()

        # Specifying forcing and evalution data paths
        sys.path.append(setup.config.build_folder)
        from SimPHony import Simulation_Single_Swiss
        from SimPHony import DateTime

        # Read in the Swiss tree data for plotting and analysis
        parser = SwissTreeParser(get_trees_psi_leaf_folder_path_cc())

        from src.core.py.Parameters import Parameters
        from src.core.py.Parameters import SoilLayer
        from src.core.py.Parameters import Soil_Water_Model_Type
        from src.core.py.Parameters import Convert_Soil_Parameters

        error_list = []

        for index in range(0,2):
            # Create parameter setup
            params = Parameters()

            nsoil_layers = 3
            layer = SoilLayer()
            layer.k_soil_sat = 1.0 / 100.0 / 86400.0
            layer.psi_soil_sat = -0.5 * 1
            # layer.camp_b  = 10.4
            layer.theta_s = 0.48
            layer.theta_r = 0.05
            layer.pore_size_ind = 0.6

            # Copy the soil layer and assume all layers have the same properties...
            soil_layers = []
            for s in range(nsoil_layers):
                soil_layers.append(copy.deepcopy(layer))

            # ... but not the depth
            soil_layers[0].depth = 0.1
            soil_layers[1].depth = 0.3
            soil_layers[2].depth = 0.4

            # ------------------------------------------------------
            # Parameter setup
            # ------------------------------------------------------

            Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

            params.canopy_height = 31
            params.huber_value = 1.0 / 3000.0
            params.k_xylem_sat = 5 * 1000 / 18
            params.stem_hydraulic_capacitance = 150 * 1000 / 18
            params.leaf_hydraulic_capacitance = 0.01 * 1000 / 18
            params.g_bark = 0.01
            params.g0 = 0.005
            params.g1 = 1.5
            params.leaf_area_index = 4.8
            params.psi_leaf_50_close = -2.3
            params.d_50_close = 2.0
            params.psi50_xylem = -3.5;
            params.psi88_xylem = -5.5;
            params.root_area_index = 4.5
            params.jackson_root_beta = 0.96
            params.tree_density = 64.0 / 10000.0
            params.anet_max = 2.5
            params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
            params.sw_rad_max = 1040
            params.soil_profile_index = index

            cparameters = params.Create_CParameters(soil_layers=soil_layers)

            # ----------------------------------------------------------------
            # PHS model simulation
            # ----------------------------------------------------------------
            # Set up the PHS simulation
            # read in the parameter file that we just created
            sim = Simulation_Single_Swiss()
            sim.Read_config(setup.config.config_file)
            sim.Init_parameters(cparameters)
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

            error_list.append(errors)

        for i in range(len(error_list[0])):
                self.assertNotEqual(error_list[0][i], error_list[1][i])

        print_sucess("Done!")
