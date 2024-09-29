import numpy as np
import os
import pandas as pd
import matplotlib.pyplot as plt
import datetime
import matplotlib.dates as mdates

import sys
sys.path.append('/Users/pp/Documents/Repos/plant_hydro_standalone/cmake-build-release')
sys.path.append('/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/py/')


from src.Parameters import Parameters
from src.Parameters import Soil_Water_Model_Type
from src.Parameters import Stem_Flow_Model_Type
from src.Parameters import Conductivity_Fraction_Module_Type
from contrib.parameter_parser import Parameter_Parser
from scipy.stats import qmc

def rescale(x, min, max):
    return min + x * (max - min)

def rescale_mean(x, mean, percent):
    min = mean * (100.0 - percent)/100.0
    max = mean * (100.0 + percent)/100.0
    return rescale(x, min, max)

class Subslicer:
    def __init__(self, array):
        self.array = array
        self.i = -1
    def get(self):
        self.i +=1
        return self.array[self.i]

ncombs = 100000
path = '/Net/Groups/BSI/work_scratch/ppapastefanou/simulations/plant_hydraulics_standalone/2024/swiss/constraind_more_k/input/'
path = '/Users/pp/data/Simulations/A08_Hydraulics_standalone/2024/Major_update/swiss/local/base/input/'

if not os.path.exists(path):
    os.makedirs(path)
seed   = 123456789
sampler = qmc.LatinHypercube(d = 21, seed= seed)
sample = sampler.random(n = ncombs)
sample = sample.T


sel_cols = []

slicer = Subslicer(array=sample)

sand_fracs = np.zeros((3, ncombs))
sand_fracs[0]        = rescale(slicer.get(), min=0.02, max = 0.15)
sand_fracs[1]        = rescale(slicer.get(), min=0.20, max = 0.3)
sand_fracs[2]        = rescale(slicer.get(), min=0.17, max = 0.33)

sel_cols.append("sand_frac_01")
sel_cols.append("sand_frac_02")
sel_cols.append("sand_frac_03")

clay_fracs = np.zeros((3, ncombs))
clay_fracs[0]        = rescale(slicer.get(), min=0.55, max = 0.68)
clay_fracs[1]        = rescale(slicer.get(), min=0.27, max = 0.38)
clay_fracs[2]        = rescale(slicer.get(), min=0.25, max = 0.31)

sel_cols.append("clay_frac_01")
sel_cols.append("clay_frac_02")
sel_cols.append("clay_frac_03")

k_soil_sats_log = np.zeros((3, ncombs))
k_soil_sats_log[0]        = rescale(slicer.get(), min= -7.5, max =-5.5)
k_soil_sats_log[1]        = k_soil_sats_log[0] - 8
k_soil_sats_log[2]        = k_soil_sats_log[0] - 9

sel_cols.append("k_soil_sat_01")
sel_cols.append("k_soil_sat_02")
sel_cols.append("k_soil_sat_03")

g0_s            = rescale(slicer.get(), min=0.03, max = 0.1)
sel_cols.append("g0")
g1_s            = rescale(slicer.get(), min = 0.5, max = 4.5)
sel_cols.append("g1")
k_xylems_sats_log   = rescale(slicer.get(), min=np.log10(1.5 * 30* 5000/100), max=np.log10(1.5 * 30 * 5000 / 1))
sel_cols.append("k_xylem_sat")

huber_values    =  rescale(slicer.get(), min = 1/6000, max= 1/4000)
sel_cols.append("huber_value")

cstem_s_log         = rescale(slicer.get(), min=np.log10(0.5 *1000.0/18.0), max=np.log10(0.5 * 200 *1000.0/18.0))
sel_cols.append("kappa_stem")
lai_s           = rescale(slicer.get(), min= 2 , max = 6)
sel_cols.append("lai")
cleaf_s         = rescale(slicer.get(), min = 0.0001*55.0, max = 0.002*55.0)
#cleaf_s         = rescale(slicer.get(), min = 0.04, max = 0.4)
sel_cols.append("kappa_leaf")
d_50close_s     = rescale(slicer.get(), min = 1.0, max = 5.0)
sel_cols.append("d50_close")
psi_50_close_s  = rescale(slicer.get(), min = -2.3, max = -1.5)
sel_cols.append("psi50_close")

jackson_s       = rescale(slicer.get(), min = 0.80, max = 0.96)
sel_cols.append("root_beta")

loc_index    = rescale(slicer.get(), min = 0, max = 8)
sel_cols.append("soil_profile_index")

psi50_xylems    = rescale(slicer.get(), min = -3.7, max = -3.4)
sel_cols.append("psi_50_xylem")

psi88_xylems_offset    = rescale(slicer.get(), min = 0.6, max = 1.0)
sel_cols.append("psi_88_xylem")

root_area_indexes    = rescale(slicer.get(), min = 5, max = 20)
sel_cols.append("root_area_indexes")

plist = Parameter_Parser()
for i in range(ncombs):
    params = Parameters()

    params.id = i
    params.dts = 1800.0

    params.root_area_index = root_area_indexes[i]

    params.soil_depths = np.array([0.1, 0.3, 0.4])
    params.soil_depths = np.array2string(params.soil_depths, separator=';')
    params.soil_depths = params.soil_depths[1:-1]

    params.k_soil_sats = params.array_to_list_entry(10**k_soil_sats_log[:,i])
    params.sand_fracs = params.array_to_list_entry(sand_fracs[:,i])
    params.clay_fracs = params.array_to_list_entry(clay_fracs[:,i])

    params.stem_hydraulic_capacitance = 10**cstem_s_log[i]
    params.leaf_area_index = lai_s[i]
    params.g0 = g0_s[i]
    params.g1 = g1_s[i]

    params.psi50_xylem = psi50_xylems[i]
    params.psi88_xylem = psi50_xylems[i] - psi88_xylems_offset[i]
    params.k_xylem_sat = 10.0**k_xylems_sats_log[i]
    params.leaf_hydraulic_capacitance = cleaf_s[i]
    params.huber_value = huber_values[i]

    params.psi_leaf_50_close = psi_50_close_s[i]
    params.d_50_close = d_50close_s[i]
    params.jackson_root_beta = jackson_s[i]

    params.canopy_height = 30

    params.tree_density = 100 / 10000
    pressure = 1.013 * 100000.0  # Pa
    c_a = 415

    params.soil_water_model_type = Soil_Water_Model_Type.Saxton06.name

    params.stem_flow_type = Stem_Flow_Model_Type.Linear.name

    params.conductivity_fraction_type = Conductivity_Fraction_Module_Type.Weibull.name

    params.soil_profile_index = int(loc_index[i])

    if i % 50000 == 0:
        print(i/ncombs * 100.0)

    params.sustain_xylem_damage =False
    params.verbose = False

    params.max_psi_leaf_change_per_hour = 1.0

    plist.Add(params)

plist.Write_Full_Parameter_File(f"{path}SwissParameterList_03_05{ncombs}.csv")
plist.Write_Partial_Parameter_File(f"{path}SwissPartialParameterList_03_05{ncombs}.csv", sel_cols)


# from hydro_standalone import Simulation_Multi
# from hydro_standalone import DateTime
# forcing_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/Meteo_Hainich_dT30min_forcing_PHS.csv"
# sap_file = "/Users/pp/Documents/Repos/plant_hydro_standalone/appl/hainich/SAP_Hainich_Fagus-mean_dT30min_prog.csv"
# paramfile = f"ParameterList{ncombs}.csv"
#
#
# df_sap = pd.read_csv(sap_file)
# df_sap['datetime']  = pd.to_datetime(df_sap['datetime'])
# # Convert from kg H2O to mol H2O
# df_sap['J'] = df_sap['J'] * 1000
# df_sap['J'] = df_sap['J'] * 1.0/18
#
# ranges = np.arange(0, ncombs)
#
# sim = Simulation_Multi_Hainich()
# sim.Init_Full_Parameter_Setups(paramfile, ranges.tolist())
# sim.Init_input(forcing_file, sap_file, 0)
# sim.Set_water_pot_initials(-1.0, -0.3)
#
# # in seconds
# steplen = 1800
#
# format = "%Y-%m-%d %H:%M:%S"
# timestart = DateTime("2023-6-17 00:00:00", format)
# timeend   = DateTime("2023-7-2 00:00:00", format)
#
# timestart = DateTime("2023-6-1 00:00:00", format)
# timeend   = DateTime("2023-10-1 00:00:00", format)
#
# # timestart = DateTime("2023-09-1 00:00:00", format)
# # timeend   = DateTime("2023-10-1 00:00:00", format)
#
# # timestart = DateTime("2023-09-1 00:00:00", format)
# # timeend   = DateTime("2023-9-3 00:00:00", format)
#
# # timestart = DateTime("2023-5-1 00:00:00", format)
# # timeend   = DateTime("2023-11-1 00:00:00", format)
#
# sim.Run(steplen, timestart, timeend)
# an = sim.Get_analysis_list()
#
# list_errors = np.zeros((ncombs,3))
#
# for i in range(0, ncombs):
#     a = an[i]
#     x = a.Get_Rmse_J()
#     y = a.Get_Rmse_G()
#     list_errors[i,0] = ranges[i]
#     list_errors[i,1] = x
#     list_errors[i,2] = y
#
# df =pd.DataFrame(list_errors, columns=['rank', 'J' ,'R'])
# df.to_csv("RMSE.csv", index=False)
#
# x = 3;
