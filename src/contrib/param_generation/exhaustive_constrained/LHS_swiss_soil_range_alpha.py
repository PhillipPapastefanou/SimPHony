import os
import numpy as np
import pandas as pd
import copy

from src.core.py.Parameters import Parameters
from src.core.py.Parameters import SoilLayer
from src.core.py.Parameters import Soil_Water_Model_Type
from src.core.py.Parameters import Stem_Flow_Model_Type
from src.core.py.Parameters import Conductivity_Fraction_Module_Type
from src.contrib.parameter_parser import Parameter_Parser
from src.core.py.Parameters import Convert_Soil_Parameters
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

def Calculate_LHS_alpha(rank, ncombs, parameters: Parameters, alpha, parameter_file):

    KG_TO_MOL = 1000.0/18.0
    MPA_TO_HHEAD = 1000.0/9.81

    seed   = 12345 * rank + 1321
    sampler = qmc.LatinHypercube(d = 20, seed= seed)
    sample = sampler.random(n = ncombs)
    sample = sample.T

    sel_cols = []
    slicer = Subslicer(array=sample)

    k_soil_sats_logs = rescale(slicer.get(), min=-7.0, max = -6.0)
    psi_soil_sats = rescale(slicer.get(), min=-0.039, max = -0.019) # 1 sigma
    #psi_soil_sats = rescale(slicer.get(), min=-0.035, max = -0.007)  # 2 sigmas
    psi_soil_sats *= MPA_TO_HHEAD
    pore_size_ind = rescale(slicer.get(), min=0.242, max = 0.301) # 1 sigma
    #pore_size_ind = rescale(slicer.get(), min=0.216, max = 0.325)  # 2 sigmas
    #sigma_wcont    = rescale(slicer.get(), min = -1, max = 1)
    sigma_wcont    = rescale(slicer.get(), min = -1, max = 0)


    soil_collection = []
    for i in range(ncombs):
        soil_collection.append(SoilLayer())

    for i in range(ncombs):
        soil_collection[i].k_soil_sat = 10**k_soil_sats_logs[i]
        soil_collection[i].psi_soil_sat = psi_soil_sats[i]
        soil_collection[i].theta_s = 0.54
        soil_collection[i].theta_r = 0.15
        soil_collection[i].pore_size_ind = pore_size_ind[i]

    sel_cols.append("k_soil_sat_01")
    sel_cols.append("psi_soil_sat_01")
    sel_cols.append("psi_01")
    sel_cols.append("sigma_wconts")

    g0_s            = rescale_mean(slicer.get(), mean = parameters.g0, percent=alpha)
    sel_cols.append("g0")

    g1_s            = rescale_mean(slicer.get(), mean = parameters.g1, percent=alpha)
    sel_cols.append("g1")

    g_barks =  rescale_mean(slicer.get(), mean = parameters.g_bark, percent=alpha)
    sel_cols.append("g_barks")

    anet_max     = rescale_mean(slicer.get(), mean = parameters.anet_max, percent=alpha)
    sel_cols.append("anet_max")

    k_xylems_sats   = rescale_mean(slicer.get(), mean = parameters.k_xylem_sat, percent=alpha)
    #k_xylems_sats *= KG_TO_MOL
    sel_cols.append("k_xylem_sat")

    huber_values    =  rescale_mean(slicer.get(), mean = parameters.huber_value, percent=alpha)
    sel_cols.append("huber_value")

    cstem_s         = rescale_mean(slicer.get(), mean = parameters.stem_hydraulic_capacitance, percent=alpha)
    #cstem_s *= KG_TO_MOL
    sel_cols.append("kappa_stem")

    # lai_s           = rescale(slicer.get(), min= 4.6, max = 5.0)
    # sel_cols.append("lai")

    cleaf_s        = rescale_mean(slicer.get(), mean = parameters.leaf_hydraulic_capacitance, percent=alpha)
    sel_cols.append("kappa_leaf")

    d_50close_s     = rescale_mean(slicer.get(), mean = parameters.d_50_close, percent=alpha)
    sel_cols.append("d50_close")

    psi_50_close_s  = rescale(slicer.get(), min = -2.3, max = -2.0)
    sel_cols.append("psi50_close")

    jackson_s       = rescale(slicer.get(), min= 0.90, max = 0.99)
    sel_cols.append("root_beta")

    psi50_xylems    = rescale(slicer.get(), min = -4.2, max = -3.5)
    sel_cols.append("psi_50_xylem")

    psi88_xylems_offset    = rescale(slicer.get(), min = 1.0, max = 1.5)
    sel_cols.append("psi_88_xylem")

    root_area_indexes    = rescale_mean(slicer.get(), mean = parameters.root_area_index, percent=alpha)
    sel_cols.append("root_area_indexes")

    # tree_densities    = rescale(slicer.get(), min = (64-32)/10000, max = (64)/10000)
    # sel_cols.append("tree_densities")

    plist = Parameter_Parser()
    for i in range(ncombs):
        params = Parameters()

        params.id = i
        params.root_area_index = root_area_indexes[i]
        soil_layer_top = soil_collection[i]

        soil_layers = []
        # Duplicate the top layer
        soil_layers.append(soil_layer_top)
        soil_layers.append(copy.deepcopy(soil_layer_top))
        soil_layers.append(copy.deepcopy(soil_layer_top))

        soil_layers[0].depth = 0.1
        soil_layers[1].depth = 0.3
        soil_layers[2].depth = 0.4

        soil_layers[1].psi_soil_sat += -0.06*MPA_TO_HHEAD
        soil_layers[2].psi_soil_sat += -0.06*MPA_TO_HHEAD

        soil_layers[1].pore_size_ind += 0.04
        soil_layers[2].pore_size_ind += 0.04

        Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

        params.stem_hydraulic_capacitance = cstem_s[i]
        # params.leaf_area_index = lai_s[i]
        params.g0 = g0_s[i]
        params.g_bark = g_barks[i]
        params.g1 = g1_s[i]
        params.anet_max = anet_max[i]

        params.psi50_xylem = psi50_xylems[i]
        params.psi88_xylem = psi50_xylems[i] - psi88_xylems_offset[i]

        if params.psi50_xylem < params.psi88_xylem:
            params.psi88_xylem = params.psi50_xylem - 0.5

        params.k_xylem_sat = k_xylems_sats[i]
        params.leaf_hydraulic_capacitance = cleaf_s[i]
        params.huber_value = huber_values[i]

        params.psi_leaf_50_close = psi_50_close_s[i]
        params.d_50_close = d_50close_s[i]
        params.jackson_root_beta = jackson_s[i]

        params.wcont_sigma_deviation = sigma_wcont[i]

        #params.soil_profile_index = soil_profile_indexes[i]
        params.soil_profile_index = 0

        params.tree_density = 10/1000

        params.canopy_height = 35

        pressure = 1.013 * 100000.0  # Pa
        c_a = 415

        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.stem_flow_type = Stem_Flow_Model_Type.Linear.name
        #params.conductivity_fraction_type = Conductivity_Fraction_Module_Type.Logit

        params.sustain_xylem_damage = False
        params.sw_rad_max = 972.935

        # if i % 5000 == 0:
        #     print(f"{i/ncombs * 100.0}% completed")

        plist.Add(params)

    plist.Write_Full_Parameter_File(f"{parameter_file}{rank}")
    #plist.Write_Partial_Parameter_File(f"{parameter_file}{rank}", sel_cols)
