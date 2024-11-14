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

root_path = "/Net/Groups/BSI/scratch/ppapastefanou/simulations/SimPHony/swiss_cc"
scenario = "ex_con_higer_soil_1000_10percent_some_params_fixed"

setup = Setup()
setup.Apply_default_swiss(Swiss_soil_water_input_type.NLayers_Mean_N_Std)
setup.Apply_default_paths(root_path, scenario)
setup.config.parameters_list_file = os.path.join(setup.config.input_path, "parameters.csv")
os.makedirs(setup.config.input_path, exist_ok=True)
setup.config.config_file = os.path.join(setup.config.input_path, "config_py.txt")
setup.config.nsims = 50000
setup.config.nbest = -1
setup.Export()

ncombs_per_parameter = 1000
alpha = 10


sys.path.append(setup.config.build_folder)
from src.contrib.param_generation.exhaustive_constrained.parallel_setup_EX_CON import ParallelSetupEXCON
from mpi4py import MPI

# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# Create the MPI binding object
binder = ParallelSetupEXCON(comm, rank, size)

# Latin hypercube parameter samples
binder.ncombs_per_parameter = ncombs_per_parameter
# Maximum deviation from the plant hydraulic parameters [%]
binder.alpha = alpha
#
binder.init(config=setup.config)
#
binder.send_parameter_indexes()
#
binder.start_simulations()
#
binder.receive_analysis_data()