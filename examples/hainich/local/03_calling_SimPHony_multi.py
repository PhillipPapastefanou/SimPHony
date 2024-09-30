#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""SimPHONY model
   Author: Phillip Papastefanou
   Email: pp.papastefanou@gmail.com
   Date: September 2024
   Version: 0.5
   Description: This script demonstrates the multi-setup functionality of the SimPHony library.
   It performs an ensemble of simulations, comparing simulated sapflow (J) and stem water potential (psi_stem) to observed data.
   Specifically, the model runs 200 simulations with identical parameters, varying only k_xylem.
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
from src.contrib.auxil.files import get_forcing_hainich
from src.contrib.auxil.files import get_sapflow_obs_hainich
from src.contrib.auxil.files import get_psi_stem_obs_hainich
from src.contrib.parameter_parser import Parameter_Parser
from src.contrib.auxil.output_df import create_output_df


found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
sys.path.append(cpp_lib_path)

# Importing local libraries and paths
from SimPHony import Simulation_Multi_Hainich
from SimPHony import DateTime

file_forcing, df_forcing = get_forcing_hainich()
file_sap_obs, df_sap_obs = get_sapflow_obs_hainich()
file_psi_stem_obs, df_psi_stem_obs = get_psi_stem_obs_hainich()

# ------------------------------------------------------
# Parameter setup
# ------------------------------------------------------

plist = Parameter_Parser()
parameter_filename = 'parameter_example_multi.csv'

# Provide a list of xylem hydraulic conductivities between
# 0.1 and 50 mol H2O m-1 s-1 MPa-1
k_xylem_list = np.arange(0.25, 50, 0.25)

for k_xylem in k_xylem_list:

    # Create parameter setup
    params = Parameters()

    nsoil_layers = 3
    layer = SoilLayer()
    layer.k_soil_sat = 0.4 / 100.0 / 86400.0
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

    # ------------------------------------------------------
    # Parameter setup
    # ------------------------------------------------------

    Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

    params.canopy_height = 31
    params.huber_value = 1.0 / 3000.0
    params.k_xylem_sat = k_xylem
    params.stem_hydraulic_capacitance = 150 * 1000 / 18
    params.leaf_hydraulic_capacitance = 0.01 * 1000 / 18
    params.g_bark = 0.01
    params.g0 = 0.01
    params.g1 = 1.5
    params.leaf_area_index = 4.8
    params.psi_leaf_50_close = -2.3
    params.d_50_close = 2.0
    params.psi50_xylem = -3.5;
    params.psi88_xylem = -5.5;
    params.root_area_index = 4.5
    params.jackson_root_beta = 0.96
    params.tree_density = 34.0 / 10000.0
    params.anet_max = 2.5
    params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
    params.sw_rad_max = 1040
    plist.Add(params)

plist.Write_Full_Parameter_File(filename=parameter_filename)

# ----------------------------------------------------------------
# SimPHony model simulation
# ----------------------------------------------------------------

# Setting up the simulation
sim = Simulation_Multi_Hainich()
sim.Init_Full_Parameter_Setups(parameter_filename, np.arange(plist.df.shape[0]))
sim.Init_input(file_forcing, file_sap_obs, file_psi_stem_obs, 0)

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

# Getting the raw analysis list
an_list = sim.Get_analysis_list()

# Print RMSE of sapflow J and psi_stem
# Enable this if you want some more specific output
# for i in range(len(an_list)):
#     print(f"Sim {i}: Sapflow_RMSE: {an_list[i].Get_Rmse_J()}")
#     print(f"Sim {i}: Psi_stem_RMSE: {an_list[i].Get_Rmse_psi_stem()}")

# Create a dataframe with the RMSEs
df_an = pd.DataFrame(columns=['RMSE_J', 'RMSE_Psi_Stem'])

for i in range(len(an_list)):
    df_an.loc[i]= [an_list[i].Get_Rmse_J(),an_list[i].Get_Rmse_psi_stem()]

df_an['k_stem'] = k_xylem_list

df_best_J = df_an.loc[df_an['RMSE_J'].idxmin()].round(8)
df_best_psi_stem = df_an.loc[df_an['RMSE_Psi_Stem'].idxmin()].round(8)

print(f"Best sapflow RMSE ({df_best_J['RMSE_J']}) with k_stem : {df_best_J['k_stem']}")
print(f"Best psi_stem RMSE ({df_best_psi_stem['RMSE_Psi_Stem']}) with k_stem : {df_best_psi_stem['k_stem']}")

import matplotlib.pyplot as plt

fig = plt.figure(figsize=(6, 10))

ax = fig.add_subplot(3, 1, 1)
ax.plot(df_an['k_stem'], df_an['RMSE_Psi_Stem'])
ax.set_xlabel('k_stem')
ax.set_ylabel('RMSE_psi_stem [MPa]')

ax = fig.add_subplot(3, 1, 2)
ax.plot(df_an['k_stem'], df_an['RMSE_J'])
ax.set_xlabel('k_stem')
ax.set_ylabel('RMSE_J [mol m-2 s-1]')

ax = fig.add_subplot(3, 1, 3)
df_sel = df_an[['RMSE_Psi_Stem', 'RMSE_J']]
df_sel = (df_sel - df_sel.min()) / (df_sel.max() - df_sel.min())
ax.plot(df_an['k_stem'], df_sel['RMSE_Psi_Stem'] + df_sel['RMSE_J'])
ax.set_xlabel('k_stem')
ax.set_ylabel('RMSE_J_normal + RMSE_Psi_Stem_normal')
plt.savefig(os.path.join(THIS_DIR, 'plt', "03_FIT.png"))

