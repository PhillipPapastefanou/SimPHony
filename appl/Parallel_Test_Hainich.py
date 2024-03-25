import sys

sys.path.append("/Users/pp/Documents/Repos/plant_hydro_standalone/")

from contrib.config import Config
config = Config()
config.build_path = '/Users/pp/Documents/Repos/plant_hydro_standalone/cmake-build-release/'
config.lib_path = '/Users/pp/Documents/Repos/plant_hydro_standalone/'
config.forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/Meteo_Hainich_dT30min_forcing_PHS.csv"
config.sap_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/SAP_Hainich_Fagus-mean_dT30min_prog.csv"
config.parameter_input_file_list = f"ParameterList{1200}.csv"

sys.path.append(config.build_path)
from contrib.ParallelRun import Binder

from mpi4py import MPI


# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()


binder = Binder(comm, rank, size)
binder.init(config=config)
binder.send_parameter_indexes()
binder.start_simulations()
binder.receive_analysis_data()