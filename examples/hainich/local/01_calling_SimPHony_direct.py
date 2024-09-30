#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""SimPHONY model
   Author: Phillip Papastefanou
   Email: pp.papastefanou@gmail.com
   Date: September 2024
   Version: 0.5
   Description: This Python script demonstrates how to directly control SimPHony,
    allowing you to customize its parameters and run simulations
   Automated plotting routines will display some of the output in the plt folder.
"""

import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.auxil.files import get_forcing_hainich
from src.contrib.auxil.files import get_sapflow_obs_hainich
from src.contrib.auxil.files import get_psi_stem_obs_hainich
from src.contrib.auxil.output_df import create_output_df
from src.contrib.auxil.output_plotter import std_plot, eval_plot_24
from src.contrib.auxil.output_plotter import eval_plot
from src.contrib.auxil.output_plotter import eval_plot_all

found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
sys.path.append(cpp_lib_path)

# Importing local libraries and paths
from SimPHony import Simulation_Single_Hainich
from SimPHony import DateTime

from src.core.py.Parameters import Parameters
from src.core.py.Parameters import SoilLayer
from src.core.py.Parameters import Soil_Water_Model_Type
from src.core.py.Parameters import Convert_Soil_Parameters

file_forcing, df_forcing = get_forcing_hainich()
file_sap_obs, df_sap_obs = get_sapflow_obs_hainich()
file_psi_stem_obs, df_psi_stem_obs = get_psi_stem_obs_hainich()

# ------------------------------------------------------
# Parameter setup
# ------------------------------------------------------

# Create parameters
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
soil_layers[0].depth = 0.08
soil_layers[1].depth = 0.16
soil_layers[2].depth = 0.32

Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

params.canopy_height = 31
params.huber_value = 1.0 / 3000.0
params.k_xylem_sat = 7 * 1000 / 18
params.stem_hydraulic_capacitance = 150 * 1000 / 18
params.leaf_hydraulic_capacitance = 0.01 * 1000 / 18
params.g_bark = 0.01
params.g0 = 0.02
params.g1 = 1.5
params.leaf_area_index = 4.8
params.psi_leaf_50_close = -2.3
params.d_50_close = 2.0
params.psi50_xylem = -3.5
params.psi88_xylem = -5.5
params.root_area_index = 4.5
params.jackson_root_beta = 0.96
params.tree_density = 64.0 / 10000.0
params.anet_max = 2.5
params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
params.sw_rad_max = 1040

cparameters = params.Create_CParameters(soil_layers=soil_layers)

# ----------------------------------------------------------------
# SimPHony model simulation
# ----------------------------------------------------------------

# Setting up the simulation
sim = Simulation_Single_Hainich()
sim.Init_parameters(cparameters)
sim.Init_input(file_forcing, file_sap_obs, file_psi_stem_obs)

# Specify initial leaf and stem water potential
sim.Set_water_pot_initials(-1.0, -0.2)

# Specify Start and End of the Simulation
format = "%Y-%m-%d %H:%M:%S"
date_start_str = "2023-04-01 00:00:00"
date_end_str = "2023-11-01 00:00:00"
timestart = DateTime(date_start_str, format)
timeend = DateTime(date_end_str, format)

# Running SimPHony
sim.Run(timestart, timeend)

# Getting the raw output data
output = sim.Get_output()
# Getting the raw analysis data
an = sim.Get_analysis()

# Creating pandas dataframe
df = create_output_df(output, date_start_str)

# ----------------------------------------------------------------
# Plotting model output
# ----------------------------------------------------------------

std_plot(df = df,
         timebegin= date_start_str,
         timeend  = date_end_str,
         path = os.path.join(THIS_DIR, 'plt','01_std_plot.png'))

std_plot(df = df,
         timebegin="2023-07-09 00:00:00",
         timeend="2023-07-27 00:00:00",
         path = os.path.join(THIS_DIR, 'plt','01_std_plot_jul.png'))

eval_plot_24(df, df_sap_obs, df_psi_stem_obs, an,
             path =  os.path.join(THIS_DIR, 'plt','01_eval_24_plot.png'),
             timebegin= "2023-07-09 00:00:00",
             timeend= "2023-07-27 00:00:00",
             )