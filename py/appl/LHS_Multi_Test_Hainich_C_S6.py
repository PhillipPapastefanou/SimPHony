import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import datetime
import matplotlib.dates as mdates

import sys
sys.path.append('/Users/pp/Documents/Repos/plant_hydro_standalone/cmake-build-release')
sys.path.append('/Users/pp/Documents/Repos/plant_hydro_standalone/')

from hydro_standalone import Simulation_Multi_Hainich
from hydro_standalone import DateTime

from src.py.Parameters import Parameters
from src.py.Parameters import Soil_Water_Model_Type
from contrib.ParametersList import ParametersList
from scipy.stats import qmc

def rescale(x, min, max):
    return min + x * (max - min)

def rescale_mean(x, mean, percent):
    min = mean * (100.0 - percent)/100.0
    max = mean * (100.0 + percent)/100.0
    return rescale(x, min, max)

ncombs = 50000
seed   = 123456789
sampler = qmc.LatinHypercube(d = 14, seed= seed)
sample = sampler.random(n = ncombs)
sample = sample.T

sand_fracs          = rescale(sample[0], min=0.025, max = 0.035)
clay_fracs          = rescale(sample[1],min = 0.49, max = 0.55)
org_matter_fracs    = rescale(sample[2], min = 0.025, max = 0.065)

cstem_s         = rescale(sample[3], min=0.5, max=1000)
lai_s           = rescale_mean(sample[4], mean=4.8, percent=30)
g0_s            = rescale(sample[5], min=0, max = 0.1)
k_xylems_sats   = rescale(sample[6], min=5, max=1000)

huber_values    =  rescale(sample[7], min = 1/8000, max= 1/3000)

cleaf_s         = rescale_mean(sample[8], mean=1.0, percent=50)
d_50close_s     = rescale(sample[10], min = 1.0, max = 3.0)
g1_s            = rescale(sample[11], min = 0.5, max = 2.0)
jackson_s       = rescale(sample[12], min = 0.94, max = 0.99)

# From Saxton et al 2006 in mm h-1
k_min = 0.1
k_max = 1000.0
# Convert to m s-1
k_min /= (1000 * 3600)
k_max /= (1000 * 3600)
k_soil_s_log    = rescale(sample[13], min = np.log10(k_min), max = np.log10(k_max))

plist = ParametersList()
for i in range(ncombs):
    params = Parameters()
    params.huber_value = 1.0 / 1500.0
    params.canopy_height = 30.0
    params.g0 = 0.01;
    params.g1 = 4.5
    params.psi_leaf_50_close = -2.1
    params.d_50_close = 2.0
    params.leaf_area_index = 4.8
    params.leaf_hydraulic_capacitance = 2.0
    params.k_xylem_sat = 30

    params.root_area_index = 24

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
    # params.camp_b = b_s[i]
    # params.camp_psi_soil_ref = psi_ref_s[i]

    params.k_soil_sat = 10**k_soil_s_log[i]
    params.sand_frac = sand_fracs[i]
    params.clay_frac = clay_fracs[i]
    params.organic_matter_frac = org_matter_fracs[i]

    params.stem_hydraulic_capacitance = cstem_s[i]
    params.leaf_area_index = lai_s[i]
    params.g0 = g0_s[i]
    params.g1 = g1_s[i]

    params.k_xylem_sat = k_xylems_sats[i]
    params.leaf_hydraulic_capacitance = cleaf_s[i]
    params.huber_value = huber_values[i]

    params.d_50_close = d_50close_s[i]
    params.jackson_root_beta = jackson_s[i]

    params.tree_density = 100 / 10000
    pressure = 1.013 * 100000.0  # Pa
    c_a = 415


    params.soil_water_model_type_enum = Soil_Water_Model_Type.Saxton06
    params.soil_water_model_type = params.soil_water_model_type_enum.name

    plist.Add(params)

plist.Write_Full_Parameter_File(f"ParameterList{ncombs}.csv")

forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/Meteo_Hainich_dT30min_forcing_PHS.csv"
sap_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/SAP_Hainich_Fagus-mean_dT30min_prog.csv"
paramfile = f"ParameterList{ncombs}.csv"


df_sap = pd.read_csv(sap_file)
df_sap['datetime']  = pd.to_datetime(df_sap['datetime'])
# Convert from kg H2O to mol H2O
df_sap['J'] = df_sap['J'] * 1000
df_sap['J'] = df_sap['J'] * 1.0/18

ranges = np.arange(0, ncombs)

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

# timestart = DateTime("2023-09-1 00:00:00", format)
# timeend   = DateTime("2023-9-3 00:00:00", format)

# timestart = DateTime("2023-5-1 00:00:00", format)
# timeend   = DateTime("2023-11-1 00:00:00", format)

sim.Run(steplen, timestart, timeend)
an = sim.Get_analysis_list()

list_errors = np.zeros((ncombs,3))

for i in range(0, ncombs):
    a = an[i]
    x = a.Get_Rmse_J()
    y = a.Get_Rmse_G()
    list_errors[i,0] = ranges[i]
    list_errors[i,1] = x
    list_errors[i,2] = y

df =pd.DataFrame(list_errors, columns=['rank', 'J' ,'R'])
df.to_csv("RMSE.csv", index=False)

x = 3;