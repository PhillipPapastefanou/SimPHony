import sys
rtpath = '/Users/pp/Documents/Repos/plant_hydro_standalone/'

sys.path.append(rtpath + 'py')

from contrib.config import Config
from contrib.config import Location

config = Config()
config.build_path = rtpath +'cpp/cmake-build-release'
config.lib_path = rtpath + 'py'
config.forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Forcing_Inter.csv";
config.theta_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Water_content_MultiSoils.csv";
config.tree_folder_path = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Trees";
config.location = Location.Swiss

#config.parameter_input_file_list = f"ParameterList{20000000}.csv"
config.parameter_input_file_list = f"/Users/pp/data/Simulations/A08_Hydraulics_standalone/2024/Major_update/swiss/local/base/input/SwissParameterList_03_05100000.csv";

sys.path.append(config.build_path)
#from contrib.ParallelRunFull import ParallelSetupFull

from contrib.ParallelSetupIndividualOut import ParallelSetupIndividual
from mpi4py import MPI

# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()


binder = ParallelSetupIndividual(comm, rank, size)
binder.init(config=config)
binder.send_parameter_indexes()
binder.start_simulations()
binder.receive_analysis()