#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""SimPHONY model
   Author: Phillip Papastefanou
   Email: pp.papastefanou@gmail.com
   Date: September 2024
   Version: 0.5
   Description: This script demonstrates the multi-setup functionality of the SimPHony library.
   It performs an ensemble of simulations, comparing simulated sapflow (J) and stem water potential (psi_stem) to observed data.
   Specifically, the model runs 300 simulations with identical parameters, varying only k_xylem.
"""

import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

from src.core.py.Parameters import Parameters
from src.core.py.Parameters import SoilLayer
from src.core.py.Parameters import Soil_Water_Model_Type
from src.core.py.Parameters import Convert_Soil_Parameters

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.auxil.files import get_forcing_swiss_cc
from src.contrib.auxil.files import get_trees_psi_leaf_cc
from src.contrib.auxil.files import get_soil_water_swiss_cc
from src.contrib.parameter_parser import Parameter_Parser
from src.contrib.auxil.output_df import create_output_df

found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
sys.path.append(cpp_lib_path)

# Importing local libraries and paths
from SimPHony import Simulation_Multi_Swiss
from SimPHony import DateTime

forcing_file, forcing_df = get_forcing_swiss_cc()
soil_water_file, soil_water_df = get_soil_water_swiss_cc()
tree_path, dummy = get_trees_psi_leaf_cc()

# ------------------------------------------------------
# Parameter setup
# ------------------------------------------------------

plist = Parameter_Parser()
parameter_filename = 'parameter_example_multi.csv'

# Provide a list of sigma water content deviations

wcont_sigma_deviations = np.arange(-1.5, 1.5, 0.01)

for wcont_sigma_dev in wcont_sigma_deviations:

    # Create parameter setup
    params = Parameters()

    nsoil_layers = 3
    layer = SoilLayer()
    layer.k_soil_sat = 0.4 / 100.0 / 86400.0
    layer.psi_soil_sat = -0.05 * 1
    # layer.camp_b  = 10.4
    layer.theta_s = 0.55
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

    params.canopy_height = 35
    params.huber_value = 1.0 / 3000.0
    params.k_xylem_sat = 5 * 1000/18.0
    params.stem_hydraulic_capacitance = 150 * 1000 / 18
    params.leaf_hydraulic_capacitance = 0.01 * 1000 / 18
    params.g_bark = 0.005
    params.g0 = 0.005
    params.g1 = 1.5
    params.leaf_area_index = 4.8
    params.psi_leaf_50_close = -2.1
    params.d_50_close = 2.0
    params.psi50_xylem = -3.5;
    params.psi88_xylem = -5.5;
    params.root_area_index = 4.5
    params.jackson_root_beta = 0.96
    params.tree_density = 34.0 / 10000.0
    params.anet_max = 2.5
    params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
    params.sw_rad_max = 1040

    params.wcont_sigma_deviation = wcont_sigma_dev
    plist.Add(params)

plist.Write_Full_Parameter_File(filename=parameter_filename)

# ----------------------------------------------------------------
# SimPHony model simulation
# ----------------------------------------------------------------

# Setting up the simulation
sim = Simulation_Multi_Swiss()
sim.Init_Full_Parameter_Setups(parameter_filename, np.arange(plist.df.shape[0]))
sim.Init_input(soil_water_file, forcing_file, tree_path, 0)

# Specify initial leaf and stem water potential
sim.Set_water_pot_initials(-1.0, -0.2)

# Specify Start and End of the Simulation
format = "%Y-%m-%d %H:%M:%S"
date_start_str = "2018-05-01 00:00:00"
date_end_str = "2018-12-15 00:00:00"
timestart = DateTime(date_start_str, format)
timeend = DateTime(date_end_str, format)

# Running SimPHony
sim.Run(timestart, timeend)

# Getting the raw analysis list
an_list = sim.Get_analysis_list()

# Print RMSE of sapflow J and psi_stem
# Enable this if you want some more specific output
# for i in range(len(an_list)):
#     print(f"Sim {i}: Sapflow_RMSE: {an_list[i].Get_Rmse_J()}")
#     print(f"Sim {i}: Psi_stem_RMSE: {an_list[i].Get_Rmse_psi_stem()}")


data = np.zeros((len(an_list), 14))
for i in range(len(an_list)):
    data[i]= an_list[i].Get_rmse()

range_alive = np.array([1, 4, 9, 10, 11, 12, 13])
range_dead = np.array([0, 3, 6, 7])

data_alive = data.T[range_alive]
data_dead = data.T[range_dead]

# Create a dataframe with the RMSEs
df_an = pd.DataFrame()

df_an = pd.DataFrame(data_dead.T)
df_an.columns = [f'd{i}' for i in range_dead]

data_alive_mean = np.mean(data_alive, axis=0)

df_an['a'] = data_alive_mean
df_an['sigma'] = wcont_sigma_deviations


import matplotlib.pyplot as plt
fig = plt.figure(figsize=(10, 10))

ax = fig.add_subplot(2, 3, 1)
ax.plot(df_an['sigma'], df_an['a'])
ax.set_xlabel('sigma')
ax.set_ylabel('RMSE')

x = 2
for i in range_dead:
    ax = fig.add_subplot(2, 3, x)
    ax.plot(df_an['sigma'], df_an[f'd{i}'], c = 'tab:red')
    ax.set_xlabel('sigma')
    ax.set_ylabel('RMSE')
    x += 1

plt.savefig(os.path.join(THIS_DIR, 'plt', "03_FIT.png"))


# df_an['k_stem'] = k_xylem_list
#
# df_best_J = df_an.loc[df_an['RMSE_J'].idxmin()].round(8)
# df_best_psi_stem = df_an.loc[df_an['RMSE_Psi_Stem'].idxmin()].round(8)
#
# print(f"Best sapflow RMSE ({df_best_J['RMSE_J']}) with k_stem : {df_best_J['k_stem']}")
# print(f"Best psi_stem RMSE ({df_best_psi_stem['RMSE_Psi_Stem']}) with k_stem : {df_best_psi_stem['k_stem']}")
#
# import matplotlib.pyplot as plt
#

# ax = fig.add_subplot(3, 1, 2)
# ax.plot(df_an['k_stem'], df_an['RMSE_J'])
# ax.set_xlabel('k_stem')
# ax.set_ylabel('RMSE_J [mol m-2 s-1]')
#
# ax = fig.add_subplot(3, 1, 3)
# df_sel = df_an[['RMSE_Psi_Stem', 'RMSE_J']]
# df_sel = (df_sel - df_sel.min()) / (df_sel.max() - df_sel.min())
# ax.plot(df_an['k_stem'], df_sel['RMSE_Psi_Stem'] + df_sel['RMSE_J'])
# ax.set_xlabel('k_stem')
# ax.set_ylabel('RMSE_J_normal + RMSE_Psi_Stem_normal')
# plt.savefig(os.path.join(THIS_DIR, 'plt', "03_FIT.png"))

