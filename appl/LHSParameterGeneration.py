import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import datetime
import matplotlib.dates as mdates

import sys
rtpath = '/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone'

sys.path.append(rtpath)

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

ncombs = 20000000
#ncombs = 10000
seed   = 123456789
sampler = qmc.LatinHypercube(d = 14, seed= seed)
sample = sampler.random(n = ncombs)
sample = sample.T

sel_cols = []

sand_fracs          = rescale(sample[0], min=0.025, max = 0.035)
clay_fracs          = rescale(sample[1],min = 0.49, max = 0.55)
org_matter_fracs    = rescale(sample[2], min = 0.025, max = 0.065)

sel_cols.append("sand_frac")
sel_cols.append("clay_frac")
sel_cols.append("organic_matter_frac")

cstem_s         = rescale(sample[3], min=0.5, max=1000)
lai_s           = rescale_mean(sample[4], mean=4.8, percent=30)
g0_s            = rescale(sample[5], min=0, max = 0.1)

sel_cols.append("stem_hydraulic_capacitance")
sel_cols.append("leaf_area_index")
sel_cols.append("g0")


k_xylems_sats   = rescale(sample[6], min=np.log10(1), max=np.log10(1000))
huber_values    =  rescale(sample[7], min = 1/8000, max= 1/3000)

sel_cols.append("huber_value")
sel_cols.append("k_xylem_sat")

cleaf_s         = rescale_mean(sample[8], mean=1.0, percent=50)
d_50close_s     = rescale(sample[10], min = 1.0, max = 3.0)
g1_s            = rescale(sample[11], min = 0.5, max = 2.0)
jackson_s       = rescale(sample[12], min = 0.94, max = 0.99)

sel_cols.append("leaf_hydraulic_capacitance")
sel_cols.append("d_50_close")
sel_cols.append("g1")
sel_cols.append("jackson_root_beta")
# From Saxton et al 2006 in mm h-1
k_min = 0.1
k_max = 1000.0
# Convert to m s-1
k_min /= (1000 * 3600)
k_max /= (1000 * 3600)
k_soil_s_log    = rescale(sample[13], min = np.log10(k_min), max = np.log10(k_max))

sel_cols.append("k_soil_sat")
tree_dens_s    = rescale(sample[9], min = 100, max = 200)
sel_cols.append("tree_density")

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

    params.k_xylem_sat = 10**k_xylems_sats[i]
    params.leaf_hydraulic_capacitance = cleaf_s[i]
    params.huber_value = huber_values[i]

    params.d_50_close = d_50close_s[i]
    params.jackson_root_beta = jackson_s[i]

    params.tree_density = tree_dens_s[i] / 10000
    pressure = 1.013 * 100000.0  # Pa
    c_a = 415

    params.soil_water_model_type_enum = Soil_Water_Model_Type.Saxton06
    params.soil_water_model_type = params.soil_water_model_type_enum.name
    params.soil_water_model_type = params.soil_water_model_type_enum.name

    if i % 50000 == 0:
        print(i/ncombs * 100.0)
    plist.Add(params)

plist.Write_Full_Parameter_File(f"ParameterList{ncombs}.csv")
plist.Write_Partial_Parameter_File(f"PartialParameterList{ncombs}.csv", sel_cols)

