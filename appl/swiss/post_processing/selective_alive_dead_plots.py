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

root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss/medlyn_fix"
scenario_name = "red_ksuper_low_1E8"

setup = Setup()
setup.Apply_default_swiss(Swiss_soil_water_input_type.NLayers_Mean_N_Std)
setup.Apply_default_paths(root_output_directory, scenario_name)
setup.config.parameters_list_file = os.path.join(setup.config.post_path, "parameters_best_mean_alive.csv")
setup.config.config_file = os.path.join("plt", "config_dummy.txt")
# Specifying forcing and evalution data paths
setup.Export()

# Read in the Swiss trees
tree_parser = SwissTreeParser(get_trees_psi_leaf_folder_path_cc())

sys.path.append(setup.config.build_folder)
from SimPHony import Simulation_Single_Swiss
from SimPHony import DateTime

selective_range = ['mean_alive', 0, 3, 6, 7]
#selective_range = [0, 4, 6, 7]
ax_indexes = np.arange(1,6)

fig = plt.figure(figsize=(12, 10))

for id, ai in zip(selective_range, ax_indexes) :

    setup.config.parameters_list_file = os.path.join(setup.config.post_path, f"parameters_best_{id}.csv")
    setup.Export()
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
    df_mod = create_output_df(output, date_start_str)

    # ----------------------------------------------------------------
    # Plotting model output
    # ----------------------------------------------------------------

    if id == 'mean_alive':
        tree_df = tree_parser.df_alive_all
    else:
        tree_df = tree_parser.trees[id].df

    ax = fig.add_subplot(3,2, ai)
    ax.scatter(tree_df.index, tree_df['xylem_pressure'], color= 'black', zorder =2)
    ax.plot(df_mod['psiLeaf'], zorder =1, c = 'tab:green')
    ax.plot(df_mod['psiStem'], zorder =1, c = 'tab:brown')
    ax.set_ylim((-7.5,0))
plt.tight_layout()
plt.savefig(os.path.join(setup.config.post_path, "Best_single_fit.png"), dpi = 150)

# std_plot(df = df,
#          timebegin= date_start_str,
#          timeend  = date_end_str,
#          path = os.path.join(THIS_DIR, 'plt','01_std_plot.png'))
#
# std_plot(df = df,
#          timebegin="2018-07-01 00:00:00",
#          timeend="2018-10-01 00:00:00",
#          path = os.path.join(THIS_DIR, 'plt','01_std_plot_jul.png'))


