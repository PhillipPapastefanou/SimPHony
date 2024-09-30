#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""SimPHONY model
   Author: Phillip Papastefanou
   Email: pp.papastefanou@gmail.com
   Date: September 2024
   Version: 0.5
   Description: This Python example demonstrates how to use a parameter file to control SimPHony.
   A sample parameter file (parameter_setup.csv) will be created and then read by the model.
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
from src.contrib.param_generation.example_generator import create_example_file
from src.contrib.auxil.output_df import create_output_df
from src.contrib.auxil.output_plotter import std_plot, eval_plot_24
from src.contrib.auxil.output_plotter import eval_plot
from src.contrib.auxil.output_plotter import eval_plot_all


found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
sys.path.append(cpp_lib_path)

# Importing local libraries and paths
from SimPHony import Simulation_Single_Hainich
from SimPHony import DateTime

file_forcing, df_forcing = get_forcing_hainich()
file_sap_obs, df_sap_obs = get_sapflow_obs_hainich()
file_psi_stem_obs, df_psi_stem_obs = get_psi_stem_obs_hainich()

# ------------------------------------------------------
# Parameter setup
# ------------------------------------------------------

# Create an example parameter file and export it to this directory
parameter_file = os.path.join(THIS_DIR, "parameter_example.csv")
create_example_file(parameter_file)

# ----------------------------------------------------------------
# SimPHony model simulation
# ----------------------------------------------------------------

# Setting up the simulation
sim = Simulation_Single_Hainich()
sim.Init_parameters_fn_single(parameter_file, 0)
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
         path = os.path.join(THIS_DIR, 'plt','02_std_plot.png'))

std_plot(df = df,
         timebegin="2023-07-09 00:00:00",
         timeend="2023-07-27 00:00:00",
         path = os.path.join(THIS_DIR, 'plt','02_std_plot_jul.png'))

eval_plot_24(df, df_sap_obs, df_psi_stem_obs, an,
             path =  os.path.join(THIS_DIR, 'plt','02_eval_24_plot.png'),
             timebegin= "2023-07-09 00:00:00",
             timeend= "2023-07-27 00:00:00",
             )