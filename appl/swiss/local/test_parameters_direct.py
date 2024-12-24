import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime
import matplotlib.pyplot as plt
from setuptools.command.setopt import config_file


THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.swiss_tree_parser import SwissTreeParser
from src.contrib.auxil.setups import Setup
from src.contrib.auxil.output_df import create_output_df
from src.contrib.auxil.files import get_trees_psi_leaf_folder_path_cc
from src.contrib.config import Config, Swiss_soil_water_input_type
from src.contrib.parameter_parser import Parameter_Parser

root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss"
scenario_name = "11_06_nstdn_change_1E7"

setup = Setup()
setup.Apply_default_swiss(Swiss_soil_water_input_type.NLayers_Mean_N_Std)
setup.Apply_default_paths(root_output_directory, scenario_name)
#etup.config.parameters_list_file = os.path.join(setup.config.post_path, "parameters_best_0.csv")
setup.config.config_file = os.path.join("plt", "config_dummy_2.txt")
# Specifying forcing and evalution data paths
setup.Export()


# Read in the Swiss trees
tree_parser = SwissTreeParser(get_trees_psi_leaf_folder_path_cc())

sys.path.append(setup.config.build_folder)
from SimPHony import Simulation_Single_Swiss
from SimPHony import DateTime

# For this example we will only look at the alive trees
parameter_file = os.path.join("/Users/pp/data/Simulations/A08_SimPHony/swiss/medlyn_fix/base_red/post/parameters_best_mean_alive.csv")
parameter_file = os.path.join("/Users/pp/data/Simulations/A08_SimPHony/swiss/ex_con_test/output/parameters_f2.csv0")
parameter_file = os.path.join("/Users/pp/data/Simulations/A08_SimPHony/swiss/excon/10000_10p/post/parameters_best_5.csv")
parameter_parser = Parameter_Parser()
# Specify number of rows to not read in the whole file
parameter_parser.Read_Parameter_List(parameter_file, nrows= 5)


# Obtain one parameter setup of the list
parameters = parameter_parser.parameters_list[4]


x = 3
# Change parameters here --------
parameters.k_xylem_sat = 0.6 * 1000/18
parameters.stem_hydraulic_capacitance = 5 * 1000/18
#parameters.wcont_sigma_deviation = -1.0
#
# parameters.psi50_xylem = -4.1
# parameters.psi88_xylem = -5.1

# parameters.wcont_sigma_deviation  = -0.5
#
parameters.g0 = 0.005
parameters.g_bark = 0.005 * 3
# parameters.g1 = 1.5
parameters.anet_max = 4


# parameters.g0 += 0.0001
# #
# parameters.g_bark  = 0.005
# # #parameters.g0 = 0.00111_06_nstdn_change_1E7
# #
# # parameters.anet_max = 1.5
# #
# #
#parameters.stem_hydraulic_capacitance= 20 * 1000/18.0

# parameters.k_xylem_sat = 2 * 1000/18.0
#
# parameters.wcont_sigma_deviation = 0
# #
# parameters.minimum_psi_leaf_multiplier = 2.0
# #
#parameters.k_xylem_sat = 5* 1000/18
#
# parameters.max_psi_leaf_change_per_hour = 0.1
# #
soil_layers = parameters.Create_soil_layers()
# # Change soil layer parameters here ---
# #
soil_layers[0].k_soil_sat = 8E-6
soil_layers[0].pore_size_ind = 0.25
soil_layers[0].theta_r = 0.10
#
soil_layers[1].theta_r = 0.15
soil_layers[1].k_soil_sat = soil_layers[0].k_soil_sat
soil_layers[2].k_soil_sat = soil_layers[0].k_soil_sat

# soil_layers[0].theta_r = 0.04
# # soil_layers[0].theta_s = 0.6
# # soil_layers[1].theta_r = 0.08
# # soil_layers[2].theta_r = 0.08

# Generate a the cpp version of the parameters to be used directly with the model
cparams = parameters.Create_CParameters(soil_layers)

# ----------------------------------------------------------------
# PHS model simulation
# ----------------------------------------------------------------
# Specify Start and End of the Simulation
format = "%Y-%m-%d %H:%M:%S"
date_start_str = "2018-05-01 00:00:00"
date_end_str = "2018-12-15 00:00:00"
timestart = DateTime(date_start_str, format)
timeend = DateTime(date_end_str, format)


# Set up the PHS simulation
# read in the parameter file that we just created
sim = Simulation_Single_Swiss()
sim.Read_config(setup.config.config_file)
sim.Init_eval(timestart, timeend)
sim.Init_parameters(cparams)
sim.Init_input()
sim.Set_water_pot_initials(-1.0, -0.2)

# Run the simulation
sim.Run(timestart, timeend)

sim.Analyse()

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

f, ax = plt.subplots()
#plt.scatter(tree_parser.df_alive_all.index, tree_parser.df_alive_all['xylem_pressure'])
plt.scatter(tree_parser.trees[7].df.index,
            tree_parser.trees[7].df['xylem_pressure'], color='black', zorder = 2 )
plt.text(0.1, 0.1, f"RMSE = {np.round(errors[7],5)}", transform=ax.transAxes)
plt.plot(df['psiLeaf'])
plt.plot(df['psiStem'])
plt.ylim(-7.5, 0 )
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


