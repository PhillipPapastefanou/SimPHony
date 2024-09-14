import sys
rtpath = '/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/'

sys.path.append(rtpath + 'py')

from contrib.config import Config
from contrib.config import Location

config = Config()
config.build_path = rtpath +'cpp/build'
config.lib_path = rtpath + 'py'
config.forcing_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Forcing_Inter.csv"
config.theta_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Water_content_MultiSoils.csv"
config.tree_folder_path = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Trees"
config.location = Location.Swiss

#config.parameter_input_file_list = f"ParameterList{20000000}.csv"

config.parameter_input_file_list = f"/Net/Groups/BSI/work_scratch/ppapastefanou/simulations/plant_hydraulics_standalone/2024/swiss/2024_soil_water_updates/TSwissParameterListWide_24_1000000.csv"

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
