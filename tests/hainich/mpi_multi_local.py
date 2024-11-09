import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir))

from src.contrib.auxil.messaging import print_sucess
from src.contrib.auxil.setups import Setup
from src.contrib.config import Config, Swiss_soil_water_input_type
from src.contrib.param_generation.example_generator import create_example_swiss_cc_parameter_list


root_output_directory = THIS_DIR
scenario_name = "test"

# Specifying forcing and evalution data paths
setup = Setup()
setup.Apply_default_hainich()
setup.Apply_default_paths(root_output_directory, scenario_name)
setup.config.parameters_list_file = os.path.join(setup.config.input_path, "parameters_example_2.csv")
setup.config.config_file = os.path.join(setup.config.input_path, "config_multi_py.txt")
setup.Export()

sys.path.append(setup.config.build_folder)
from src.contrib.parallel_setup import ParallelSetup
from mpi4py import MPI

# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# Create the MPI binding object
binder = ParallelSetup(comm, rank, size)
# 
binder.init(config=setup.config)
#
binder.send_parameter_indexes()
#
binder.start_simulations()
#
binder.receive_analysis_data()