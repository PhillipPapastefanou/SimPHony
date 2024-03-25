import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import datetime
import matplotlib.dates as mdates

import sys
sys.path.append('/Users/pp/Documents/Repos/plant_hydro_standalone/cmake-build-release')
from hydro_standalone import Simulation_Multi_Hainich
from hydro_standalone import DateTime

from contrib.Parameters import Parameters
from contrib.ParametersList import ParametersList

plist = ParametersList()
nfiles = 100;
for i in range(0,nfiles):
    params = Parameters()
    params.huber_value = 1.0 / 1500.0
    params.canopy_height = 30.0
    params.g0 = 0.01;
    params.g1 = 4.5
    params.psi_leaf_50_close = -2.1
    params.d_50_close = 2.0
    params.leaf_area_index = 4.8
    params.leaf_hydraulic_capacitance = 2.0
    params.stem_hydraulic_capacitance = i * 1000 / 18.0  * 2+ 1
    params.k_xylem_sat = 30

    params.root_area_index = 10

    params.soil_depths = np.array([0.08, 0.16, 0.32])
    params.soil_depths = np.array2string(params.soil_depths, separator=';')
    params.soil_depths = params.soil_depths[1:-1]

    params.jackson_root_beta = 0.96
    params.theta_r = 0.0972
    params.alpha_genucht = 1
    params.n_genucht = 5
    params.neta_genucht = 0.5

    # params.theta_s = 0.426
    # params.b = 10.4

    params.camp_psi_soil_ref = -4.5 * 1000 * 1E-6
    params.k_soil_sat = 0.05 / 100.0 / 3600.0 * i
    params.theta_s = 0.52
    params.camp_b = 3.2
    params.tree_density = 100 / 10000

    pressure = 1.013 * 100000.0  # Pa
    c_a = 415

    plist.Add(params)
plist.Write_Full_Parameter_File(f"ParameterList{nfiles}.csv")




forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/Meteo_Hainich_dT30min_forcing_PHS.csv"
sap_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/SAP_Hainich_Fagus-mean_dT30min_prog.csv"
paramfile = f"ParameterList{nfiles}.csv"


df_sap = pd.read_csv(sap_file)
df_sap['datetime']  = pd.to_datetime(df_sap['datetime'])
# Convert from kg H2O to mol H2O
df_sap['J'] = df_sap['J'] * 1000
df_sap['J'] = df_sap['J'] * 1.0/18

ranges = np.arange(0,nfiles)

sim = Simulation_Multi_Hainich()
sim.Init_Full_Parameter_Setups(paramfile, ranges.tolist())
sim.Init_input(forcing_file, sap_file, 0)
sim.Set_water_pot_initials(-1.0, -0.3)

# in seconds
steplen = 1800

format = "%Y-%m-%d %H:%M:%S"
timestart = DateTime("2023-6-17 00:00:00", format)
timeend   = DateTime("2023-7-2 00:00:00", format)

timestart = DateTime("2023-6-1 00:00:00", format)
timeend   = DateTime("2023-10-1 00:00:00", format)

# timestart = DateTime("2023-09-1 00:00:00", format)
# timeend   = DateTime("2023-10-1 00:00:00", format)
#
# timestart = DateTime("2023-09-1 00:00:00", format)
# timeend   = DateTime("2023-9-3 00:00:00", format)
#

timestart = DateTime("2023-6-1 00:00:00", format)
timeend   = DateTime("2023-8-1 00:00:00", format)

sim.Run(steplen, timestart, timeend)
an = sim.Get_analysis_list()

list_errors = np.zeros((nfiles,3))

for i in range(0, nfiles):
    a = an[i]
    x = a.Get_Rmse_J()
    y = a.Get_Rmse_G()
    list_errors[i,0] = ranges[i]
    list_errors[i,1] = x
    list_errors[i,2] = y

df =pd.DataFrame(list_errors, columns=['rank', 'J' ,'R'])
df.to_csv("Erorrs.csv", index=False)

x = 3;

#
#
#
# print(f"RMSE J: {x}")
# print(f"RMSE G: {y}")
# #
# times = output.Get_times()
#
# formatter = mdates.DateFormatter('%m-%d %H');
#
# fig = plt.figure(figsize=(10,10))
#
# df = pd.DataFrame(times, columns= ['DeltaT'])
#
# df_swc_raw = pd.read_csv(forcing_file)
# dates = df_swc_raw['datetime'][:-1]
#
# offset_date_str  = dates[0]
# offset_date_str = offset_date_str.replace('T', ' ')
# offset_date_str = offset_date_str.replace('Z', '')
# offset_date = datetime.datetime.strptime(offset_date_str, '%Y-%m-%d %H:%M:%S')
#
# df['date'] = [offset_date + datetime.timedelta(seconds = int(i)) for i in df['DeltaT'].values]
# df.reset_index()
# df.set_index('date')
#
# df['vpd'] = output.Get_vpd()
# df['anet'] = output.Get_anet()
#
# df['ksSoilUp'] = np.array(output.Get_ks_soil())[:, 0];
#
# df['psiSoilUp'] = np.array(output.Get_psi_soil_indiv())[:, 0];
# df['psiStem'] = output.Get_psi_stem()
# df['psiLeaf'] = output.Get_psi_leaf()
#
# df['T'] = output.Get_T()
# df['G'] = output.Get_G()
# df['J'] = output.Get_J()
#
# df['Gs'] = output.Get_G_per_sap()
# df['Js'] = output.Get_J_per_sap()
# df.set_index('date', inplace = True)
#
# ax = fig.add_subplot(3,2,1)
# ax.plot(df['vpd'], label = 'VPD', c= 'tab:red')
# ax.legend()
# ax.set_xlabel("Time")
# ax.tick_params(axis='x', labelrotation=45)
# ax.xaxis.set_major_formatter(formatter)
# ax.set_xlim((df.index[0]), (df.index[-1]))
#
# ax = fig.add_subplot(3,2,2)
# ax.plot(df['anet'], label = 'Anet')
# ax.legend()
# ax.set_xlabel("Time")
# ax.tick_params(axis='x', labelrotation=45)
# ax.xaxis.set_major_formatter(formatter)
# ax.set_xlim((df.index[0]), (df.index[-1]))
#
# ax = fig.add_subplot(3,2,3)
# ax.semilogy(df['ksSoilUp'], label = 'ksoilup')
# ax.legend()
# ax.set_xlabel("Time")
# ax.tick_params(axis='x', labelrotation=45)
# ax.xaxis.set_major_formatter(formatter)
# ax.set_xlim((df.index[0]), (df.index[-1]))
#
#
# ax = fig.add_subplot(3,2,4)
# ax.plot(df['psiLeaf'], label = 'Leaf')
# ax.plot(df['psiStem'], label = 'Stem')
# ax.plot(df['psiSoilUp'], label = 'SoilT')
# ax.legend()
# ax.set_ylabel("Water potentials [MPa]")
# ax.set_xlabel("Time")
# ax.tick_params(axis='x', labelrotation=45)
# ax.xaxis.set_major_formatter(formatter)
# ax.set_xlim((df.index[0]), (df.index[-1]))
#
# ax = fig.add_subplot(3,2,5)
# ax.plot(df['T'], label = 'T', c= 'tab:blue')
# ax.plot(df['J'], label = 'J',  c= 'tab:orange')
# ax.plot(df['G'], label = 'G', c = 'black')
# ax.legend()
# ax.set_ylabel("Water flows")
# ax.set_xlabel("Time")
# ax.tick_params(axis='x', labelrotation=45)
# ax.xaxis.set_major_formatter(formatter)
# ax.set_xlim((df.index[0]), (df.index[-1]))
#
#
# ax = fig.add_subplot(3,2,6 )
#
# #ax.plot(df['Js'], label = 'J',  c= 'tab:orange')
# ax.plot(df['Gs'], label = 'J model', c = 'black')
# ax.plot(df_sap['datetime'], df_sap['J'], label = 'J obs', c ='tab:red')
# ax.legend()
# ax.set_ylabel("Water flow\nper sap area")
# ax.set_xlabel("Time")
# ax.tick_params(axis='x', labelrotation=45)
# ax.xaxis.set_major_formatter(formatter)
# ax.set_xlim((df.index[0]), (df.index[-1]))
#
# plt.subplots_adjust(hspace= 0.5, bottom = 0.2)
# #plt.plot(in_thetas[0:2*24*2])
# plt.tight_layout()
# plt.savefig("Water_flow_obs_model.png", dpi = 300)
# plt.show()
#
