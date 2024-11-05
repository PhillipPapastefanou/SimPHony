import sys
import os
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))

from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.config import Config, Location, Swiss_soil_water_input_type
from src.contrib.auxil.files import get_lib_directory
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.auxil.files import get_soil_water_swiss_cc
from src.contrib.auxil.files import get_forcing_swiss_cc
from src.contrib.auxil.files import get_trees_psi_leaf_cc

found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
if found_cpp_lib:
    sys.path.append(cpp_lib_path)
else:
    print("Could not find Simphony library")


scenario_name = "05_11_nlayer_nstdn_full_1E7"
root_output_directory = "/Net/Groups/BSI/scratch/ppapastefanou/simulations/SimPHony/swiss_cc"
swiss_soil_water_type = Swiss_soil_water_input_type.NLayers_Mean_N_Std

root_library_path = get_lib_directory()
foundlib, libpath, binpath = get_SimPHony_build_path()
root_data_path = os.path.join(root_library_path, os.pardir, os.pardir, 'data')

forcing_file, null = get_forcing_swiss_cc()
soilwater_file, null= get_soil_water_swiss_cc(swiss_soil_water_type)
tree_folder, null  = get_trees_psi_leaf_cc()

# Specifying forcing and evalution data paths
config = Config()
config.location = Location.Swiss_cc
config.swiss_tree_folder_path = tree_folder
config.soilwater_file = soilwater_file
config.forcing_file = forcing_file
config.swiss_soil_water_input_type = swiss_soil_water_type
config.post_path =  os.path.join(root_output_directory, scenario_name, 'post')
config.output_path =  os.path.join(root_output_directory, scenario_name, 'output')
config.input_path =   os.path.join(root_output_directory, scenario_name, 'input')
config.scenario_path =  os.path.join(root_output_directory, scenario_name)
config.parameters_list_file = os.path.join(config.input_path, 'parameters.csv')
config.nsims = 10000000
config.nbest = 50
config.build_path = binpath
config.lib_path = root_library_path
config.config_file = os.path.join(root_output_directory, scenario_name, 'input', 'config.txt')
os.makedirs(os.path.join(root_output_directory, scenario_name, 'input'), exist_ok=True)
config.Export(config.config_file)

sys.path.append(config.build_path)

from src.contrib.parallel_setup_LHS import ParallelSetupWithLHS
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