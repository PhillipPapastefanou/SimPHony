import sys
rtpath = '/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/'

sys.path.append(rtpath)

from contrib.config import Config
config = Config()
config.build_path = rtpath +'/build'
config.lib_path = '/Users/pp/Documents/Repos/plant_hydro_standalone/'
config.forcing_file = rtpath +"/appl/hainich/Meteo_Hainich_dT30min_forcing_PHS.csv"
config.sap_file = rtpath + "/appl/hainich/SAP_Hainich_Fagus-mean_dT30min_prog.csv"
config.parameter_input_file_list = f"ParameterList{20000000}.csv"
#config.parameter_input_file_list = f"ParameterList{10000}.csv"

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
binder.receive_analysis_data()