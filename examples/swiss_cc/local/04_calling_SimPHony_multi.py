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
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))


from src.contrib.config import Config
from src.contrib.config import Location
from src.contrib.auxil.files import get_lib_directory
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.auxil.files import get_soil_water_swiss_cc
from src.contrib.auxil.files import get_forcing_swiss_cc
from src.contrib.auxil.files import get_trees_psi_leaf_folder_path_cc
from src.contrib.config import Config, Location, Swiss_soil_water_input_type

root_library_path = get_lib_directory()
foundlib, libpath, binpath = get_SimPHony_build_path()

sys.path.append(libpath)
sys.path.append(os.path.join(root_library_path, 'py'))

forcing_file, null = get_forcing_swiss_cc()
soil_water_file, null = get_soil_water_swiss_cc()
tree_path, null = get_trees_psi_leaf_folder_path_cc()


# Specifying forcing and evalution data paths
config = Config()
config.location = Location.Swiss_cc
config.build_path = binpath
config.lib_path = root_library_path
config.forcing_file = forcing_file
config.swiss_tree_folder_path = tree_path
config.soilwater_file = soil_water_file
config.swiss_soil_water_input_type = Swiss_soil_water_input_type.NLayers_Indiv

root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss"
scenario_name = "test"
config.scenario_path =  os.path.join(root_output_directory, scenario_name)
config.output_path =  os.path.join(root_output_directory, scenario_name, 'output')
config.post_path =   os.path.join(root_output_directory, scenario_name, 'post')
config.input_path =   os.path.join(root_output_directory, scenario_name, 'input')
config.config_file  = os.path.join(root_output_directory, scenario_name, 'input', 'config.txt')
config.parameters_list_file = os.path.join(root_output_directory, scenario_name, 'input', "parameters.csv")

config.nsims = 20000
config.nbest = 50

config.Export(config.config_file)

sys.path.append(config.build_path)
from src.contrib.param_generation.LHS.parallel_setup_LHS import ParallelSetupWithLHS
from mpi4py import MPI

# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# Create the MPI binding object
binder = ParallelSetupWithLHS(comm, rank, size)
#
binder.init(config=config)
#
binder.send_parameter_indexes()
#
binder.start_simulations()
#
binder.receive_analysis_data()
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

