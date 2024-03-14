import numpy as np

from src.py.Leaf_Stem_Coupled_timesteps import ML_Leaf_Stem_SemiCoupled_Module
#from A08_Hydraulic_Standalone.Drougth_experiment_simulation.Model.multi_layer.Leaf_Stem_Cav_Couple import ML_Le≤af_Stem_SemiCoupled_Module
from src.py.Parameters import Parameters
#from A08_Hydraulic_Standalone.Drougth_experiment_simulation.Model.RelativeWaterContent import RelativeWaterContent

import pandas as pd
import matplotlib.pyplot as plt
import datetime
import matplotlib.dates as mdates
from time import perf_counter



# from A08_Hydraulic_Standalone.Drougth_experiment_simulation.IO.TreeWaterPotentialData import TreeWaterPotData
# from A08_Hydraulic_Standalone.Drougth_experiment_simulation.IO.TreeWaterPotentialData import Tree
# from A08_Hydraulic_Standalone.Drougth_experiment_simulation.IO.TreeWaterPotentialData import State
# tree_experimental_setup = TreeWaterPotData()

#df_swc_raw = pd.read_csv('../IO/Water_Input_org.csv')
df_swc_raw = pd.read_csv('/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_\
experiment_simulation//IO/Water_Input_type2.csv')
df_forc= pd.read_csv('/Users/pp/Dropbox/UNI/Projekte/A08_Hydraulic_Standalone/Drougth_\
experiment_simulation/IO/Forcing_Inter.csv')

dates = df_swc_raw['dt'][:-1]

in_thetas = df_swc_raw.values[:-1,1:]

# Flip the soil layers to make the first layer be the top layer
in_thetas = np.flip(in_thetas, 1)


df_forc_slice = df_forc.iloc[7249-1:17473-1]
in_vpd = df_forc_slice['vpd'].values * 1000.0 # From kPa to Pa
sw_arr = df_forc_slice['swdown'].values
dates_slz = df_forc_slice['dt']


# Length of one timer inveral simualtion in [s]
dts = 3600/2

max_swdown = 1040
anet_max = 5.7
anet_max = 12.6
anet_max = 25.6
in_anpp = sw_arr / max_swdown * anet_max

params = Parameters()
params.huber_value  = 1.0/3600.0
params.huber_value  = 1.0/1000.0
params.canopy_height  = 20.0
params.g0 = 0.005
params.g1 = 1.5
params.g1 = 3.5
params.psi_leaf_50_close = -2.1
params.d_50_close = 10.0
params.leaf_area_index = 4.8
params.leaf_hydraulic_capacitance = 1.0
params.stem_hydraulic_capacitance = 200* 1000/18.0
params.k_xylem_sat = 300
params.k_xylem_sat = 600

params.root_area_index = 24

params.jackson_root_beta = 0.96
params.theta_r = 0.0972
params.alpha_genucht = 1
params.n_genucht = 5
params.neta_genucht = 0.5


#params.theta_s = 0.426
#params.b = 10.4

params.camp_psi_soil_ref = -4.5 * 1000 * 1E-6
params.k_soil_sat = 20/100.0/3600.0
params.theta_s = 0.43
params.camp_b = 4.5

pressure = 1.013*100000.0 #Pa
c_a = 415

psi_stem_init = -0.3;
psi_leaf_init = -1.0;

model = ML_Leaf_Stem_SemiCoupled_Module(params=params)
model.set_initial_conditions(psi_leaf_init, psi_stem_init)
model.set_drivers(anet=in_anpp, c_a =c_a, pressure = pressure, vpd = in_vpd, theta=in_thetas )


# timestart =  30*2*24* 0
# timeend   =  30*2*24* 213

# in seconds
timestart = 86400 * 0
timeend   = 86400 * 20


print("Simulating...")
t1 = perf_counter()
#model.Update_Euler_Explicit(steplen, timestart, timeend)
model.Update_Euler_Implicit(steplen= dts,timestart_s = timestart, timeend_s = timeend)
t2 = perf_counter()
print(f"Done! {t2-t1}s")


elasticity = 10.0;
osmotic_pot = -2.1;

# rel_cont_module = RelativeWaterContent(elasticity = elasticity, osmotic_potential=osmotic_pot)
formatter = mdates.DateFormatter('%d-%m %H');

fig = plt.figure(figsize=(20,15))

times = model.times
df = pd.DataFrame(times, columns= ['DeltaT'])

offset_date_str  = dates[0]

offset_date = datetime.datetime.strptime(offset_date_str, '%Y-%m-%d %H:%M:%S')

df['date'] = [offset_date + datetime.timedelta(seconds = int(i)) for i in df['DeltaT'].values]
df.reset_index()
df.set_index('date')


df['vpd'] = model.out_vpds
df['vwc_lower'] = model.out_vwc_lower
df['vwc_upper'] = model.out_vwc_upper
df['psiLeaf'] = model.psi_leaves
df['psiStem'] = model.psi_stems
df['psisoilavg'] = model.psi_soil_avg

df['T'] = model.Ts
df['J'] = model.Js
df['G'] = model.Gs

df['beta'] = model.betas
df['gs'] = model.gss

df['step_stem'] = model.steps_S
df['step_leaf'] = model.steps_L

df.set_index('date', inplace = True)

ax = fig.add_subplot(4,2,1)
ax.plot(df['vpd'])
ax.set_xlabel("Time")
ax.set_ylabel("VPD [Pa]")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)

ax = fig.add_subplot(4,2,2)
ax.plot(df['vwc_upper'], label = 'Upper')
ax.plot(df['vwc_lower'], label = 'Lower')
ax.set_xlabel("Time")
ax.set_ylabel("Volumetric water content [-]")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)
ax.legend()

ax = fig.add_subplot(4,2,3)
ax.plot(df['psiLeaf'], label = 'Leaf')
ax.plot(df['psiStem'] , label = 'Stem')
ax.plot(df['psisoilavg'], label = 'Soil_avg')

# trees = tree_experimental_setup.trees
# for tree in trees:
#     if tree.state == State.Alive:
#         c = 'red'
#         #ax.plot(tree.dates[0], tree.xylem_pressure, c=c);
#         #ax.scatter(tree.dates[0], tree.xylem_pressure, c=c);


ax.legend()
ax.set_ylabel("Water potentials [MPa]")
ax.set_xlabel("Time")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)

molH20Togram = 18.025
ax = fig.add_subplot(4,2,4)
ax.plot(df['T'] * molH20Togram, label = 'T')
ax.plot(df['J'] * molH20Togram, label = 'J')
ax.plot(df['G'] * molH20Togram, label = 'G')
ax.legend()
ax.set_xlabel("Time")
ax.set_ylabel("Water fluxes [g m-2 s-1]")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)

ax = fig.add_subplot(4,2,5)
ax.plot(df['beta'])
ax.set_ylim((0,1.1))
ax.set_xlabel("Time")
ax.set_ylabel("Stomatal opening  beta [-]")
ax.tick_params(axis='x', labelrotation=45)
ax.xaxis.set_major_formatter(formatter)

ax = fig.add_subplot(4,2,6)
ax.plot(df['gs'])
ax.set_xlabel("Time")
ax.set_ylabel("Stomatal conductance [mol m-2 s-1]")
ax.xaxis.set_major_formatter(formatter)
ax.tick_params(axis='x', labelrotation=45)

ax = fig.add_subplot(4,2,7)
ax.scatter(df.index, df['step_stem'], s = 5, label = 'Stem')
ax.scatter(df.index, df['step_leaf'], s = 5, label = 'Leaf')
ax.legend()
ax.set_xlabel("Time ")
ax.set_ylabel("Bisection solver steps [-]")
ax.xaxis.set_major_formatter(formatter)
ax.tick_params(axis='x', labelrotation=45)

# df['RWC'] = [rel_cont_module.get(x) for x in model.psi_stems]
# ax = fig.add_subplot(4,2,8)
# ax.plot(df['RWC'])
# ax.set_xlabel("Time ")
# ax.set_ylabel("Relative water content [-]")
# ax.xaxis.set_major_formatter(formatter)
# ax.tick_params(axis='x', labelrotation=45)


plt.subplots_adjust(hspace= 0.5)
#plt.plot(in_thetas[0:2*24*2])
plt.show()
#plt.savefig("a.png")

#df.to_csv("ModelOutput.csv")