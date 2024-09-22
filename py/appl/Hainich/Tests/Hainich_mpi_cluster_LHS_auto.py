import sys
import os

# Specify paths so that the PHS model can be imported
root_library_path = '/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone'
root_data_path = os.path.join(root_library_path, 'data')
cpp_lib_path = os.path.join(root_library_path, 'cpp', 'build')
sys.path.append(cpp_lib_path)
sys.path.append(os.path.join(root_library_path, 'py'))

from contrib.config import Config
config = Config()
config.build_path = cpp_lib_path
config.lib_path = root_library_path
config.forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
config.sap_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
config.psi_stem_file = os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')

root_output_directory = "/Net/Groups/BSI/work_scratch/ppapastefanou/simulations/plant_hydraulics_standalone/2024/hainich"
scenario_name = "fine_vanGenuchten"
config.scenario_path =  os.path.join(root_output_directory, scenario_name)
config.output_path =  os.path.join(root_output_directory, scenario_name, 'output')
config.post_path =   os.path.join(root_output_directory, scenario_name, 'post')
config.input_path =   os.path.join(root_output_directory, scenario_name, 'input')
config.nsims = 50000*128
config.nbest = 200

sys.path.append(config.build_path)
from contrib.parallel_setup_hainich_with_LHS import ParallelSetupHainichWithLHS
from mpi4py import MPI

# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

# Create the MPI binding object
binder = ParallelSetupHainichWithLHS(comm, rank, size)
# 
binder.init(config=config)
#
binder.send_parameter_indexes()
#
binder.start_simulations()
#
binder.receive_analysis_data()