import sys
import os

# Specify paths so that the PHS model can be imported
root_library_path = '/Users/pp/Documents/Repos/plant_hydro_standalone'
root_data_path = os.path.join(root_library_path, 'data')
cpp_lib_path = os.path.join(root_library_path, 'cpp', 'cmake-build-release')
sys.path.append(cpp_lib_path)
sys.path.append(os.path.join(root_library_path, 'py'))

from contrib.config import Config
config = Config()
config.build_path = cpp_lib_path
config.lib_path = root_library_path
config.forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
config.sap_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
config.psi_stem_file = os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')

root_output_directory = "/Users/pp/data/Simulations/A08_Hydraulics_standalone/hainich"
scenario_name = "broad_local_anet_fix"
config.parameter_input_file_list = os.path.join(root_output_directory, scenario_name, 'input', 'Hainich_parameters.csv')
config.output_path =  os.path.join(root_output_directory, scenario_name, 'output')
config.scenario_path =  os.path.join(root_output_directory, scenario_name)
config.input_path =   os.path.join(root_output_directory, scenario_name, 'input')



sys.path.append(config.build_path)
from contrib.parallel_setup_hainich import ParallelSetupHainich
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