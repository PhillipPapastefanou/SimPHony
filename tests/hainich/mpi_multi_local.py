import sys
import os

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir))
from src.contrib.auxil.files import get_SimPHony_build_path

root_library_path = THIS_DIR
root_data_path = os.path.join(root_library_path, os.pardir, os.pardir, 'data')
# Specifying forcing and evalution data paths
forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
sapflux_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
psi_stem_file = os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')

found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
sys.path.append(cpp_lib_path)

from src.contrib.config import Config
config = Config()
config.build_path = cpp_lib_path
config.lib_path = root_library_path
config.forcing_file = forcing_file
config.sap_file = sapflux_file
config.psi_stem_file = psi_stem_file

root_output_directory = THIS_DIR
scenario_name = "test"

config.output_path =  os.path.join(root_output_directory, scenario_name, 'output')
config.scenario_path =  os.path.join(root_output_directory, scenario_name)
config.input_path =   os.path.join(root_output_directory, scenario_name, 'input')
config.parameter_input_file_list = os.path.join(config.input_path, 'parameter_example_2.csv')


sys.path.append(config.build_path)
from src.contrib.parallel_setup_hainich import ParallelSetupHainich
from mpi4py import MPI

# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# Create the MPI binding object
binder = ParallelSetupHainich(comm, rank, size)
# 
binder.init(config=config)
#
binder.send_parameter_indexes()
#
binder.start_simulations()
#
binder.receive_analysis_data()