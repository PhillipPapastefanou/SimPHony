import sys
import os
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir))

from src.contrib.auxil.files import get_SimPHony_build_path
from src.contrib.config import Config, Location, Swiss_soil_water_input_type

found_cpp_lib, cpp_bin_path, cpp_lib_path = get_SimPHony_build_path()
sys.path.append(cpp_lib_path)

root_library_path = THIS_DIR
root_data_path = os.path.join(root_library_path, os.pardir, os.pardir, 'data')


# Specifying forcing and evalution data paths
config = Config()
config.location = Location.Hainich
config.forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
config.sap_flow_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
config.psi_stem_file = os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')
config.config_file = os.path.join(THIS_DIR, 'test', 'input', 'config_py_multi_mpi.txt')

root_output_directory = THIS_DIR
scenario_name = "test"

config.post_path =  os.path.join(root_output_directory, scenario_name, 'post')
config.output_path =  os.path.join(root_output_directory, scenario_name, 'output')
config.input_path =   os.path.join(root_output_directory, scenario_name, 'input')
config.scenario_path =  os.path.join(root_output_directory, scenario_name)
config.parameters_list_file = os.path.join(config.input_path, 'parameter_example_2.csv')
config.Export(config.config_file)


sys.path.append(config.build_path)
from src.contrib.parallel_setup import ParallelSetup
from mpi4py import MPI

# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# Create the MPI binding object
binder = ParallelSetup(comm, rank, size)
# 
binder.init(config=config)
#
binder.send_parameter_indexes()
#
binder.start_simulations()
#
binder.receive_analysis_data()