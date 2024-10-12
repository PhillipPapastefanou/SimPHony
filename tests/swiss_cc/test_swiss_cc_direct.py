import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.auxil.messaging import print_failure ,print_sucess
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_Swiss_Direct(unittest.TestCase):
    def test_swiss_direct(self):

        print("Calling the cpp lib directly from python...", end='')
        root_library_path = THIS_DIR
        root_data_path = os.path.join(root_library_path, os.pardir, os.pardir, 'data')
        # Specifying forcing and evalution data paths
        forcing_file = os.path.join(root_data_path, 'swiss', 'input', 'Forcing_Inter.csv')
        soil_water_file = os.path.join(root_data_path, 'swiss', 'input', 'swiss_cc_soil_water_with_sd.csv')
        tree_path = os.path.join(root_data_path, 'swiss', 'eval', 'Trees')

        found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
        sys.path.append(cpp_lib_path)

        # Importing local libraries and paths
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
        sim.Init_parameters(cparameters)
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
        REFERENCE_PSI_LEAF_0_RMSE = 1.8912050337701065;
        REFERENCE_PSI_LEAF_6_RMSE = 2.5209254139403057;

        self.assertAlmostEqual(errors[0], REFERENCE_PSI_LEAF_0_RMSE, places=EPS)
        self.assertAlmostEqual(errors[6], REFERENCE_PSI_LEAF_6_RMSE, places=EPS)
        print("Done!")
