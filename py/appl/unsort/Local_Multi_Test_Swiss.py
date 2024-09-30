import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import datetime
import matplotlib.dates as mdates
import subprocess


import sys
sys.path.append('../../../cpp/cmake-build-release')
sys.path.append('../..')

from src.Parameters import Parameters


forcing_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Forcing_Inter.csv";
theta_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Water_content_MultiSoils.csv";
parameters_list = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/py/appl/LHS/generator_files/SwissPartialParameterListWide_24_1000.csv";
tree_folder_path = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Trees";


theta_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Water_content_MultiSoils.csv";
forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Forcing_Inter.csv";
#parameters_list = "/Users/pp/data/Simulations/A08_Hydraulics_standalone/2024/swiss/LHS/14-4/Best_Alive_avg.csv";
parameters_list = "LHS/generator_files/SwissParameterListWide_24_1000.csv";
tree_folder_path = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Trees";


from hydro_standalone import Simulation_Single_Swiss
from hydro_standalone import Simulation_Multi_Swiss
from hydro_standalone import DateTime



# sim = Simulation_Single_Swiss()
# sim.Init_parameters_fn_single(parameters_list, u)
# sim.Init_input(theta_file, forcing_file, tree_folder_path)
# sim.Set_water_pot_initials(-1.0, -0.3)

sim = Simulation_Multi_Swiss()
sim.Init_Full_Parameter_Setups(parameters_list, np.arange(0, 1000))
sim.Init_input(theta_file, forcing_file, tree_folder_path, 0)
sim.Set_water_pot_initials(-1.0, -0.1)


format = "%Y-%m-%d %H:%M:%S"
timestart = DateTime("2018-4-01 00:00:00", format)
timeend   = DateTime("2018-12-01 00:00:00", format)


sim.Run(timestart, timeend)
#output = sim.Get_output()
list_an  = sim.Get_analysis_list()


