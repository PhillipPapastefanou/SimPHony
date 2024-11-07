import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.swiss_tree_parser import SwissTreeParser
from src.contrib.auxil.setups import Setup
from src.contrib.auxil.output_df import create_output_df
from src.contrib.auxil.files import get_trees_psi_leaf_folder_path_cc
from src.contrib.config import Config, Swiss_soil_water_input_type

root_path = THIS_DIR
scenario = "test"

setup = Setup()
setup.Apply_default_swiss(Swiss_soil_water_input_type.NLayers_Mean_N_Std)
setup.Apply_default_paths(root_path, scenario)
setup.config.parameters_list_file = os.path.join(setup.config.post_path, "parameters_best_mean_alive.csv")
setup.config.config_file = os.path.join("plt", "config_dummy.txt")
setup.Export()

# Specifying forcing and evalution data paths
sys.path.append(setup.config.build_folder)
from SimPHony import Simulation_Single_Swiss
from SimPHony import DateTime

# Read in the Swiss tree data for plotting and analysis
parser = SwissTreeParser(get_trees_psi_leaf_folder_path_cc())

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
# read in the parameter file that we just created1
sim = Simulation_Single_Swiss()
sim.Read_config(setup.config.config_file)
sim.Init_eval(timestart, timeend)
sim.Init_parameters_fn_single(0)
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
plt.scatter(parser.df_alive_all.index, parser.df_alive_all['xylem_pressure'])
plt.plot(df['psiLeaf'])
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


