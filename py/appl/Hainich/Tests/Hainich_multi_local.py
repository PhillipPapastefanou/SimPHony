# Importing baseline libraries
import sys
import os
import numpy as np
import pandas as pd
import datetime


# Specify paths so that the PHS model can be imported
root_library_path = '/Users/pp/Documents/Repos/plant_hydro_standalone'
root_data_path = os.path.join(root_library_path, 'data')
cpp_lib_path = os.path.join(root_library_path, 'cpp', 'cmake-build-release')
sys.path.append(cpp_lib_path)

from hydro_standalone import Simulation_Multi_Hainich
from hydro_standalone import DateTime
from contrib.parameter_parser import Parameter_Parser

# Specifying forcing and evalution data paths
forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
sapflux_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
psi_stem_file = os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')

parameters_list = os.path.join(root_library_path, 'py', 'appl',
                               'LHS', 'generator_files', 'Hainich_parameters_1000.csv')


# forcing_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Forcing_Inter.csv";
# theta_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Water_content_MultiSoils.csv";
# parameters_list = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/py/appl/LHS/generator_files/SwissPartialParameterListWide_24_1000.csv";
# tree_folder_path = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Trees";


# ----------------------------------------------------------------
# PHS model simulation
# ----------------------------------------------------------------
sim = Simulation_Multi_Hainich()
sim.Init_Full_Parameter_Setups(parameters_list, np.arange(0,  1000))
sim.Init_input(forcing_file, sapflux_file, psi_stem_file, 0)
sim.Set_water_pot_initials(-1.0, -0.2)

format = "%Y-%m-%d %H:%M:%S"
timestart = DateTime("2023-04-01 00:00:00", format)
timeend   = DateTime("2023-11-01 00:00:00", format)

sim.Run(timestart, timeend)
#output = sim.Get_output()
list_an  = sim.Get_analysis_list()

i = 0
rmse_psi_stem_list = []
rmse_J_list = []
for an in list_an:
    rmse_psi_stem_list.append(an.Get_Rmse_psi_stem())
    rmse_J_list.append(an.Get_Rmse_J())

df = pd.DataFrame()
df['rmse_psi_stem'] = rmse_psi_stem_list
df['rmse_J'] = rmse_J_list
df.sort_values(by='rmse_psi_stem', inplace=True)
print(df[0:60])




