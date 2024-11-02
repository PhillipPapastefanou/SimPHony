import sys
import os
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir))


from src.contrib.config import Config, Location
from src.contrib.auxil.files import get_lib_directory
from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.auxil.files import get_soil_water_swiss_cc
from src.contrib.auxil.files import get_forcing_swiss_cc
from src.contrib.auxil.files import get_trees_psi_leaf_cc


root_library_path = get_lib_directory()
foundlib, libpath, binpath = get_SimPHony_build_path()

sys.path.append(libpath)
sys.path.append(os.path.join(root_library_path, 'py'))

forcing_file, null = get_forcing_swiss_cc()
soilwater_file, null= get_soil_water_swiss_cc()
tree_folder, null  = get_trees_psi_leaf_cc()

config = Config()
config.location = Location.Swiss_cc
config.build_path = binpath
config.lib_path = root_library_path
config.forcing_file = forcing_file
config.swiss_tree_folder_path = tree_folder
config.soilwater_file = soilwater_file

root_output_directory = "/Users/pp/data/Simulations/A08_Hydraulics_standalone/swiss"
scenario_name = "test"
config.scenario_path =  os.path.join(root_output_directory, scenario_name)
config.output_path =  os.path.join(root_output_directory, scenario_name, 'output')
config.post_path =   os.path.join(root_output_directory, scenario_name, 'post')
config.input_path =   os.path.join(root_output_directory, scenario_name, 'input')
config.nsims = 1200
config.nbest = 100

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