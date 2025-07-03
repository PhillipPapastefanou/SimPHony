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

def Calculate_parameter_list_nlayers(rank, ncombs):
    KG_TO_MOL = 1000.0/18.0
    MPA_TO_HHEAD = 1000.0/9.81

    seed   = 12345 * rank + 1321
    sampler = qmc.LatinHypercube(d = 25, seed= seed)
    sample = sampler.random(n = ncombs)
    sample = sample.T

    slicer = Subslicer(array=sample)

    soil_collection = []
    for i in range(ncombs):
        soil_collection.append(SoilLayer())

    g0_s            = rescale(slicer.get(), min=0.001, max = 0.002)
    g1_s            = rescale(slicer.get(), min =1.8, max = 2.5)
    g_barks_factor    = rescale(slicer.get(), min = 0.0, max = 0.3)
    k_xylems_sats   = rescale(slicer.get(), min = 0.5, max= 2.0)
    k_xylems_sats *= KG_TO_MOL
    huber_values    = rescale(slicer.get(), min = 1/4500, max= 1/1200)
    cstem_s         = rescale(slicer.get(), min = 20, max=180)
    cstem_s *= KG_TO_MOL
    
    vmax25s         = rescale(slicer.get(), min = 30, max = 50)
    jmax25s         = rescale(slicer.get(), min = 60, max = 80)

    # lai_s           = rescale(slicer.get(), min= 4.6, max = 5.0)
    cleaf_s_log        = rescale(slicer.get(), min=np.log10(0.001), max=np.log10(0.005))
    d_50close_s     = rescale(slicer.get(), min = 2.5, max = 4.0)
    #psi_50_close_s  = rescale(slicer.get(), min = -2.1, max = -2.2)
    jackson_s       = rescale(slicer.get(), min = 0.90, max = 0.98)
    #psi50_xylems    = rescale(slicer.get(), min = -4.0, max = -3.8)
    #psi88_xylems_offset    = rescale(slicer.get(), min = 1.0, max = 1.5)
    root_area_indexes    = rescale(slicer.get(), min = 5, max = 12)

    # tree_densities    = rescale(slicer.get(), min = (64-32)/10000, max = (64)/10000)
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

        Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

        params.stem_hydraulic_capacitance = cstem_s[i]
        # params.leaf_area_index = lai_s[i]
        params.g0 = g0_s[i]
        params.g_bark = g0_s[i]* g_barks_factor[i]
        params.g1 = g1_s[i]
        params.vmax25 = vmax25s[i]
        params.jmax25 = jmax25s[i]

        # params.psi50_xylem = psi50_xylems[i]
        # params.psi88_xylem = psi50_xylems[i] - psi88_xylems_offset[i]
        params.k_xylem_sat = k_xylems_sats[i]
        params.leaf_hydraulic_capacitance = (10**cleaf_s_log[i])*KG_TO_MOL
        params.huber_value = huber_values[i]

        #params.psi_leaf_50_close = psi_50_close_s[i]
        params.d_50_close = d_50close_s[i]
        params.jackson_root_beta = jackson_s[i]

        params.tree_density = 10/1000
        params.canopy_height = 35


        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.stem_flow_type = Stem_Flow_Model_Type.Linear.name
        #params.conductivity_fraction_type = Conductivity_Fraction_Module_Type.Logit
        params.sustain_xylem_damage = True


        plist.Add(params)
    return plist


def Calculate_parameter_list_one_layer(rank, ncombs):
    KG_TO_MOL = 1000.0/18.0
    MPA_TO_HHEAD = 1000.0/9.81

    seed   = 12345 * rank + 1321
    sampler = qmc.LatinHypercube(d = 25, seed= seed)
    sample = sampler.random(n = ncombs)
    sample = sample.T

    slicer = Subslicer(array=sample)

    soil_collection = []
    for i in range(ncombs):
        soil_collection.append(SoilLayer())

    g0_s            = rescale(slicer.get(), min= 0.001, max = 0.002)
    g1_s            = rescale(slicer.get(), min = 1.0, max = 3.0)
    g_barks_factor    = rescale(slicer.get(), min = 0.1, max = 0.3)
    k_xylems_sats   = rescale(slicer.get(), min = 0.5, max= 3.0)
    k_xylems_sats *= KG_TO_MOL
    huber_values    = rescale(slicer.get(), min = 1/4500, max= 1/2000)
    cstem_s         = rescale(slicer.get(), min = 20, max=200)
    cstem_s *= KG_TO_MOL
    
    vmax25s         = rescale(slicer.get(), min = 30, max = 50)
    jmax25s         = rescale(slicer.get(), min = 60, max = 80)

    #k_heart_saps    = rescale(slicer.get(), min = 0.00001, max = 0.0001)


    # lai_s           = rescale(slicer.get(), min= 4.6, max = 5.0)
    cleaf_s_log        = rescale(slicer.get(), min=np.log10(0.001), max=np.log10(0.0025))
    d_50close_s     = rescale(slicer.get(), min = 2.5, max = 10.0)
    psi_50_close_s  = rescale(slicer.get(), min = -1.2, max = -0.5)
    #jackson_s       = rescale(slicer.get(), min = 0.90, max = 0.98)
    #psi50_xylems    = rescale(slicer.get(), min = -4.0, max = -3.8)
    #psi88_xylems_offset    = rescale(slicer.get(), min = 1.0, max = 1.5)
    root_area_indexes    = rescale(slicer.get(), min = 5, max = 12)

    # tree_densities    = rescale(slicer.get(), min = (64-32)/10000, max = (64)/10000)
    plist = Parameter_Parser()
    for i in range(ncombs):
        params = Parameters()

        params.id = i
        params.root_area_index = root_area_indexes[i]
        soil_layer_top = soil_collection[i]

        soil_layers = []
        # Duplicate the top layer
        soil_layers.append(soil_layer_top)

        Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

        params.stem_hydraulic_capacitance = cstem_s[i]
        # params.leaf_area_index = lai_s[i]
        params.g0 = g0_s[i]
        params.g_stem_res = g0_s[i]* g_barks_factor[i]
        params.g1 = g1_s[i]
        params.vmax25 = vmax25s[i]
        params.jmax25 = jmax25s[i]
        
        #params.k_heart_sap = k_heart_saps[i]
        params.k_heart_sap = 0.0

        # params.psi50_xylem = psi50_xylems[i]
        # params.psi88_xylem = psi50_xylems[i] - psi88_xylems_offset[i]
        params.k_xylem_sat = k_xylems_sats[i]
        params.leaf_hydraulic_capacitance = (10**cleaf_s_log[i])*KG_TO_MOL
        params.huber_value = huber_values[i]

        params.psi_leaf_50_close = psi_50_close_s[i]
        params.d_50_close = d_50close_s[i]
        #params.jackson_root_beta = jackson_s[i]

        params.tree_density = 10/1000
        params.canopy_height = 35

        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.stem_flow_type = Stem_Flow_Model_Type.Linear.name
        
        params.conductivity_fraction_type = Conductivity_Fraction_Module_Type.Logit.name
        
        params.sustain_xylem_damage = True
        plist.Add(params)
    return plist 