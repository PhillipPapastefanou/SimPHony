import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import datetime
import matplotlib.dates as mdates
import subprocess


import sys
sys.path.append('../../cpp/cmake-build-release')
sys.path.append('../../py')

from src.Parameters import Parameters


forcing_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Forcing_Inter.csv";
theta_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Water_content_MultiSoils.csv";
parameters_list = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/py/appl/LHS/SwissParameterList100.csv";
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
sim.Init_Full_Parameter_Setups(parameters_list, np.arange(0, 200))
sim.Init_input(theta_file, forcing_file, tree_folder_path, 0)
sim.Set_water_pot_initials(-1.0, -0.1)


format = "%Y-%m-%d %H:%M:%S"
timestart = DateTime("2018-4-01 00:00:00", format)
timeend   = DateTime("2018-12-01 00:00:00", format)


sim.Run(timestart, timeend)
#output = sim.Get_output()
list_an  = sim.Get_analysis_list()


x = an.Get_rmse()

print(f"RMSE psiL: {x}")

times = output.Get_times()

formatter = mdates.DateFormatter('%m-%d %H');

fig = plt.figure(figsize=(10, 10))

df = pd.DataFrame(times, columns= ['DeltaT'])


offset_date_str = "2018-4-01 00:00:00"
offset_date = datetime.datetime.strptime(offset_date_str, '%Y-%m-%d %H:%M:%S')

df['date'] = [offset_date + datetime.timedelta(seconds = int(i)) for i in df['DeltaT'].values]
df.reset_index()
df.set_index('date')

df['vpd'] = output.Get_vpd()
df['anet'] = output.Get_anet()

df['ksSoilUp'] = np.array(output.Get_ks_soil())[:, 0];
df['ksSoil2'] = np.array(output.Get_ks_soil())[:, 1];
df['ksSoil3'] = np.array(output.Get_ks_soil())[:, 2];


df['psiSoilUp'] = np.array(output.Get_psi_soil_indiv())[:, 0];
df['psiStem'] = output.Get_psi_stem()
df['psiLeaf'] = output.Get_psi_leaf()

df['T'] = output.Get_T()
df['G'] = output.Get_G()
df['J'] = output.Get_J()

df['Gs'] = output.Get_G_per_sap()
df['Js'] = output.Get_J_per_sap()
df.set_index('date', inplace = True)

ax = fig.add_subplot(3,2,1)
ax.plot(df['vpd'], label = 'VPD', c= 'tab:red')
ax.legend()
ax.set_xlabel("Time")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)
ax.set_xlim((df.index[0]), (df.index[-1]))

ax = fig.add_subplot(3,2,2)
ax.plot(df['anet'], label = 'Anet')
ax.legend()
ax.set_xlabel("Time")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)
ax.set_xlim((df.index[0]), (df.index[-1]))

ax = fig.add_subplot(3,2,3)
ax.semilogy(df['ksSoilUp'], label = 'ksoil1')
ax.semilogy(df['ksSoil2'], label = 'ksoil2')
ax.semilogy(df['ksSoil3'], label = 'ksoil3')
ax.legend()
ax.set_xlabel("Time")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)
ax.set_xlim((df.index[0]), (df.index[-1]))
#ax.set_ylim((10**-16, 10**-3))


ax = fig.add_subplot(3,2,4)
ax.plot(df['psiLeaf'], label = 'Leaf')
ax.plot(df['psiStem'], label = 'Stem')
ax.plot(df['psiSoilUp'], label = 'SoilT')
ax.legend()
ax.set_ylabel("Water potentials [MPa]")
ax.set_xlabel("Time")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)
ax.set_xlim((df.index[0]), (df.index[-1]))
ax.set_ylim((-10,0))

ax = fig.add_subplot(3,2,5)
ax.plot(df['T'], label = 'T', c= 'tab:blue')
ax.plot(df['J'], label = 'J',  c= 'tab:orange')
ax.plot(df['G'], label = 'G', c = 'black')
ax.legend()
ax.set_ylabel("Water flows")
ax.set_xlabel("Time")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)
ax.set_xlim((df.index[0]), (df.index[-1]))


# ax = fig.add_subplot(3,2,6)
#
# #ax.plot(df['Js'], label = 'J',  c= 'tab:orange')
# ax.plot(df['Gs'], label = 'J model', c = 'black', alpha = 0.5)
# ax.plot(df_sap['datetime'], df_sap['J'], label = 'J obs', c ='tab:red', alpha = 0.5)
# ax.legend()
# ax.set_ylabel("Water flow\nper sap area")
# ax.set_xlabel("Time")
# ax.tick_params(axis='x', labelrotation=45)
# ax.xaxis.set_major_formatter(formatter)
# ax.set_xlim((df.index[0]), (df.index[-1]))

plt.subplots_adjust(hspace= 0.5, bottom = 0.2)
#plt.plot(in_thetas[0:2*24*2])
plt.tight_layout()
plt.savefig(f"Swiss_Water_flow_obs_model_24.png", dpi = 300)
#plt.show()

