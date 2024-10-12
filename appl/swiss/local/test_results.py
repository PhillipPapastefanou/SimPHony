import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.param_generation.example_generator import create_example_swiss_cc_file
from src.contrib.auxil.messaging import print_failure , print_sucess
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.auxil.files import get_forcing_swiss_cc
from src.contrib.auxil.files import get_trees_psi_leaf_cc
from src.contrib.auxil.files import get_soil_water_swiss_cc
from src.contrib.auxil.output_df import create_output_df
from src.contrib.auxil.output_plotter import std_plot


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

parameter_file = os.path.join("/Users/pp/data/Simulations/A08_Hydraulics_standalone/swiss/test/post/parameters_best_mean_alive.csv")

# ----------------------------------------------------------------
# PHS model simulation
# ----------------------------------------------------------------

# Set up the PHS simulation
# read in the parameter file that we just created
sim = Simulation_Single_Swiss()
sim.Init_parameters_fn_single(parameter_file, 0)
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

std_plot(df = df,
         timebegin= date_start_str,
         timeend  = date_end_str,
         path = os.path.join(THIS_DIR, 'plt','01_std_plot.png'))

std_plot(df = df,
         timebegin="2018-07-01 00:00:00",
         timeend="2018-10-01 00:00:00",
         path = os.path.join(THIS_DIR, 'plt','01_std_plot_jul.png'))


