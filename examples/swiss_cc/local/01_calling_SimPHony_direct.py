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

import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.auxil.setups import Setup
from src.contrib.auxil.output_df import create_output_df
from src.contrib.config import Config, Swiss_soil_water_input_type

root_path = "/Users/pp/data/Simulations/A08_SimPHony/swiss"
scenario = ""

setup = Setup()
setup.Apply_default_swiss(Swiss_soil_water_input_type.NLayers_Indiv)
setup.Apply_default_paths(root_path, scenario)
#setup.config.parameters_list_file = os.path.join(setup.config.post_path, "parameters_best_mean_alive.csv")
setup.config.config_file = os.path.join(THIS_DIR, "config_py.txt")
setup.Export()

sys.path.append(setup.config.build_folder)

# Importing local libraries and paths
from SimPHony import Simulation_Single_Swiss
from SimPHony import DateTime
from src.core.py.Parameters import Parameters
from src.core.py.Parameters import SoilLayer
from src.core.py.Parameters import Soil_Water_Model_Type
from src.core.py.Parameters import Convert_Soil_Parameters
from src.contrib.auxil.output_plotter import std_plot

# ------------------------------------------------------
# Parameter setup
# ------------------------------------------------------

# Create parameters
params = Parameters()

nsoil_layers = 3
layer = SoilLayer()
layer.k_soil_sat = 0.1 / 100.0 / 86400.0
layer.psi_soil_sat = -0.022 * 1000/9.81
# layer.camp_b  = 10.4
layer.theta_s = 0.6
layer.theta_r = 0.15
layer.pore_size_ind = 0.35

# Copy the soil layer and assume all layers have the same properties...
soil_layers = []
for s in range(nsoil_layers):
    soil_layers.append(copy.deepcopy(layer))

# ... but not the depth
soil_layers[0].depth = 0.1
soil_layers[1].depth = 0.3
soil_layers[2].depth = 0.4

Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

params.canopy_height = 35
params.huber_value = 1.0 / 3000.0
params.k_xylem_sat = 3.0 * 1000/18.0
params.stem_hydraulic_capacitance = 100.0 * 1000 / 18
params.leaf_hydraulic_capacitance = 0.01 * 1000 / 18
params.g_bark = 0.001
params.g0 = 0.01
params.g1 = 1.5
params.leaf_area_index = 4.8
params.psi_leaf_50_close = -2.1
params.d_50_close = 2.0
params.psi50_xylem = -3.5;
params.psi88_xylem = -5.5;
params.root_area_index = 4.5
params.jackson_root_beta = 0.96
params.tree_density = 34.0 / 10000.0
params.jmax25 = 71
params.vmax25 = 42
params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name

#params.wcont_sigma_deviation = 1.2
params.wcont_sigma_deviation = 0
params.soil_profile_index = 2

cparameters = params.Create_CParameters(soil_layers=soil_layers)

# ----------------------------------------------------------------
# SimPHony model simulation
# ----------------------------------------------------------------

# Setting up the simulation
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
         timebegin="2018-07-01 00:00:00",
         timeend="2018-07-02 00:00:00",
         path = os.path.join(THIS_DIR, 'plt','01_std_plot_jul.png'))