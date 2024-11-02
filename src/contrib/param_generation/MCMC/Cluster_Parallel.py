
import sys
rtpath_model = '/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/'
rtpath_input = '/Net/Groups/BSI/work_scratch/ppapastefanou/simulations/plant_hydraulics_standalone/2024/Hainich/MCMC/uniform_run/'

sys.path.append(rtpath_model + 'py')

ncombs = 20000000
ncombs = 20000

from contrib.config import Config
config = Config()
config.build_path = rtpath_model + 'cpp/build'
config.lib_path = rtpath_model + 'py/'
config.forcing_file = rtpath_model + "/data/hainich/Meteo_Hainich_dT30min_forcing_PHS.csv"
config.sap_file = rtpath_model + "/data/hainich/SAP_Hainich_Fagus-mean_dT30min_prog.csv"
config.parameters_list_file = f"{rtpath_input}UniformParameterList{ncombs}.csv"
config.parameter_input_file_list_partial = f"{rtpath_input}UniformPartialParameterList{ncombs}.csv"
config.output_path = "/Net/Groups/BSI/work_scratch/ppapastefanou/simulations/plant_hydraulics_standalone/2024/Hainich/MCMC/linear/"


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