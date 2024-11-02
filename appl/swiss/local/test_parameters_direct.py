import unittest
import sys
import os
import copy
from ast import parse

import numpy as np
import pandas as pd
import datetime
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.parameter_parser import Parameter_Parser
from src.core.py.Parameters import Parameters

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.auxil.files import get_forcing_swiss_cc
from src.contrib.auxil.files import get_trees_psi_leaf_cc
from src.contrib.auxil.files import get_soil_water_swiss_cc
from src.contrib.auxil.output_df import create_output_df
from src.contrib.auxil.output_plotter import std_plot

from src.contrib.swiss_tree_parser import SwissTreeParser
from src.contrib.swiss_tree_parser import Tree


forcing_file, forcing_df = get_forcing_swiss_cc()
soil_water_file, soil_water_df = get_soil_water_swiss_cc()
tree_path, dummy = get_trees_psi_leaf_cc()


sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

# Specifying forcing and evalution data paths


found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
sys.path.append(cpp_lib_path)


# Importing local libraries and paths
from SimPHony import Simulation_Single_Swiss
from SimPHony import DateTime

# Read in the Swiss trees
tree_parser = SwissTreeParser(tree_path)

# For this example we will only look at the alive trees
parameter_file = os.path.join("/Users/pp/data/Simulations/A08_SimPHony/swiss/2023swp_red_2/post/parameters_best_6.csv")
parameter_parser = Parameter_Parser()
# Specify number of rows to not read in the whole file
parameter_parser.Read_Parameter_List(parameter_file, nrows= 100)


# Obtain one parameter setup of the list
parameters = parameter_parser.parameters_list[0]

# Change parameters here --------
parameters.k_xylem_sat = 40
# parameters.stem_hydraulic_capacitance = 50 * 1000/18
parameters.wcont_sigma_deviation = -1.0

parameters.psi50_xylem = -4.1
parameters.psi88_xylem = -5.1

parameters.g_bark  = 0.001
#parameters.g0 = 0.001

parameters.anet_max = 1.5


parameters.stem_hydraulic_capacitance= 150 * 1000/18

soil_layers = parameters.Create_soil_layers()
# Change soil layer parameters here ---

soil_layers[0].k_soil_sat *= 1.5

# soil_layers[1].k_soil_sat = 2E-6
soil_layers[2].k_soil_sat = 0
#
# soil_layers[0].theta_r = 0.04
# # soil_layers[0].theta_s = 0.6
# # soil_layers[1].theta_r = 0.08
# # soil_layers[2].theta_r = 0.08

# Generate a the cpp version of the parameters to be used directly with the model
cparams = parameters.Create_CParameters(soil_layers)

# ----------------------------------------------------------------
# PHS model simulation
# ----------------------------------------------------------------

# Set up the PHS simulation
# read in the parameter file that we just created
sim = Simulation_Single_Swiss()
sim.Init_parameters(cparams)
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

# Getting the raw output data
output = sim.Get_output()
# Getting the raw analysis data
an = sim.Get_analysis()

# Creating pandas dataframe
df = create_output_df(output, date_start_str)

# ----------------------------------------------------------------
# Plotting model output
# ----------------------------------------------------------------

import matplotlib.pyplot as plt
#plt.scatter(tree_parser.df_alive_all.index, tree_parser.df_alive_all['xylem_pressure'])
plt.scatter(tree_parser.trees[6].df.index, tree_parser.trees[6].df['xylem_pressure'], color='black', zorder =2 )
plt.plot(df['psiLeaf'])
plt.plot(df['psiStem'])
plt.ylim(-7.3,0)
plt.show()

# std_plot(df = df,
#          timebegin= date_start_str,
#          timeend  = date_end_str,
#          path = os.path.join(THIS_DIR, 'plt','01_std_plot.png'))
#
# std_plot(df = df,
#          timebegin="2018-07-01 00:00:00",
#          timeend="2018-10-01 00:00:00",
#          path = os.path.join(THIS_DIR, 'plt','01_std_plot_jul.png'))


