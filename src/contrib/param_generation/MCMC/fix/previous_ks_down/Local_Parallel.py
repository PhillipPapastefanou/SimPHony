import sys

sys.path.append("//")

from contrib.config import Config
config = Config()
config.build_path = '/Users/pp/Documents/Repos/plant_hydro_standalone/cpp/cmake-build-release/'
config.lib_path = '/Users/pp/Documents/Repos/plant_hydro_standalone/'
config.forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/hainich/Meteo_Hainich_dT30min_forcing_PHS.csv"
config.sap_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/hainich/SAP_Hainich_Fagus-mean_dT30min_prog.csv"
config.parameters_list_file = f"../LHS/UniformParameterList{200000}.csv"
config.parameter_input_file_list_partial = f"../LHS/UniformPartialParameterList{200000}.csv"

sys.path.append(config.build_path)
from contrib.ParallelRunFull import ParallelSetupFull
from contrib.MCMC_Parallel_Individual import ParallelSetupIndividual

from mpi4py import MPI


# Initialize MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()


binder = ParallelSetupIndividual(comm, rank, size)
binder.init(config=config)
binder.send_parameter_indexes()
binder.start_simulations()
binder.perform_analysis()