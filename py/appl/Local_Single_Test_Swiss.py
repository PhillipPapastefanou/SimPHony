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
from src.Parameters import Stem_Flow_Model_Type
from src.Parameters import Soil_Water_Model_Type
from contrib.ParametersList import ParametersList

params = Parameters()
# params.huber_value = 1.0 / 2000.0
# params.canopy_height = 30.0
# params.g0 = 0.01;
# params.g1 = 4.5
# params.psi_leaf_50_close = -1.5
# params.d_50_close = 6.0
# params.leaf_area_index = 4.8
# params.leaf_hydraulic_capacitance = 1.0
# params.stem_hydraulic_capacitance = 100 * 1000 / 18.0
# params.k_xylem_sat = 100
#
# params.psi50_xylem = -2.5
# params.psi88_xylem = -5.5
#
# params.root_area_index = 10
# params.jackson_root_beta = 0.96

# params.soil_water_model_type_enum = Soil_Water_Model_Type.Saxton06
# params.soil_water_model_type = params.soil_water_model_type_enum.name
#
# params.stem_flow_type_enum = Stem_Flow_Model_Type.Linear
# params.stem_flow_type = params.stem_flow_type_enum.name


# params.theta_s = 0.6
# params.theta_r = 0.0972
# params.theta_r = 0.011
# params.alpha_genucht = 0.46
# params.n_genucht = 1.466
# params.neta_genucht = 0.5

# params.theta_s = 0.426
# params.b = 10.4

# params.camp_psi_soil_ref = -4.5 * 1000 * 1E-6
# params.camp_b = 3.2
#
# params.soil_depths = np.array([0.1, 0.3, 0.4])
# params.soil_depths = np.array2string(params.soil_depths, separator=';')
# params.soil_depths = params.soil_depths[1:-1]
#
# params.clay_fracs = np.array([0.5, 0.5, 0.5])
# params.clay_fracs = np.array2string(params.clay_fracs, separator=';')
# params.clay_fracs = params.clay_fracs[1:-1]
#
# params.sand_fracs = np.array([0.03, 0.03, 0.03])
# params.sand_fracs = np.array2string(params.sand_fracs, separator=';')
# params.sand_fracs = params.sand_fracs[1:-1]
#
# params.organic_matter_fracs = np.array([0.05, 0.05, 0.05])
# params.organic_matter_fracs = np.array2string(params.organic_matter_fracs, separator=';')
# params.organic_matter_fracs = params.organic_matter_fracs[1:-1]
#
params.k_soil_sats = np.array([1.0 / 100.0 / 86400.0, 1.0 / 100.0 / 86400.0, 1.0 / 100.0 / 86400.0]) * 0.1
params.k_soil_sats = np.array2string(params.k_soil_sats, separator=';')
params.k_soil_sats = params.k_soil_sats[1:-1]


params.camp_b = np.array([5,5, 5])
params.camp_b = np.array2string(params.camp_b, separator=';')
params.camp_b = params.camp_b[1:-1]

params.camp_psi_soil_ref = -123123123


#params.soil_water_model_type = Soil_Water_Model_Type.Campbell.name
params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name

params.soil_profile_index = 3;

params.canopy_height = 30

params.k_xylem_sat = 10 * 1000/18

params.stem_hydraulic_capacitance = 50* 1000/18

params.leaf_hydraulic_capacitance = 1

params.g0 = 0.015

params.leaf_area_index = 5


#params.g1 = 5.2


# params.organic_matter_frac= 0.064

pressure = 1.013 * 100000.0  # Pa
c_a = 415
params.tree_density = 100 / 10000


plist = ParametersList()
plist.Add(params)
plist.Write_Full_Parameter_File("SwissParameterList1.csv")


forcing_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Forcing_Inter.csv";
theta_file = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Water_content_MultiSoils.csv";
parameters_list = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/py/appl/LHS/SwissParameterList100.csv";
tree_folder_path = "/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/data/swiss/Trees";


theta_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Water_content_MultiSoils.csv";
forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Forcing_Inter.csv";
#parameters_list = "/Users/pp/data/Simulations/A08_Hydraulics_standalone/2024/swiss/LHS/14-4/Best_Alive_avg.csv";
parameters_list = "SwissParameterList1.csv";
tree_folder_path = "/Users/pp/Documents/Repos/plant_hydro_standalone/data/swiss/Trees";


from hydro_standalone import Simulation_Single_Swiss
from hydro_standalone import Simulation_Multi_Swiss
from hydro_standalone import DateTime
u = 0

# sim = Simulation_Single_Swiss()
# sim.Init_parameters_fn_single(parameters_list, u)
# sim.Init_input(theta_file, forcing_file, tree_folder_path)
# sim.Set_water_pot_initials(-1.0, -0.3)

sim = Simulation_Single_Swiss()
sim.Init_parameters_fn_single(parameters_list, u)
sim.Init_input(theta_file, forcing_file, tree_folder_path)
sim.Set_water_pot_initials(-1.0, -0.1)


format = "%Y-%m-%d %H:%M:%S"
timestart = DateTime("2018-4-01 00:00:00", format)
timeend   = DateTime("2018-11-01 00:00:00", format)


sim.Run(timestart, timeend)
output = sim.Get_output()
an = sim.Get_analysis()


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


df['gss'] = output.Get_gs()


df.set_index('date', inplace = True)

df  = df.loc['2018-07-1':'2018-7-29']

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
#ax.set_ylim((-10,0))

ax = fig.add_subplot(3,2,5)
ax.plot(df['T'], label = 'T', c= 'tab:blue')
ax.plot(df['J'], label = 'J', c= 'tab:orange')
ax.plot(df['G'], label = 'G', c = 'black')
ax.legend()
ax.set_ylabel("Water flows")
ax.set_xlabel("Time")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)
ax.set_xlim((df.index[0]), (df.index[-1]))

ax = fig.add_subplot(3,2,6)
ax.plot(df['gss'], label = 'gs', c= 'tab:blue')
ax.legend()
ax.set_ylabel("gs")
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
plt.savefig(f"Swiss_{params.stem_flow_type}_Water_flow_obs_model{u}.png", dpi = 300)
#plt.show()

