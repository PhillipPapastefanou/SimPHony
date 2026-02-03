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

def Calculate_LHS_per_process_swiss_cc_indiv(rank, ncombs, parameter_file):
    KG_TO_MOL = 1000.0/18.0
    MPA_TO_HHEAD = 1000.0/9.81

    seed   = 12345 * rank + 1321
    sampler = qmc.LatinHypercube(d = 25, seed= seed)
    sample = sampler.random(n = ncombs)
    sample = sample.T

    sel_cols = []
    slicer = Subslicer(array=sample)

    # Setting up soil layers
    nsoil_layers = 3

    k_soil_sats_logs = rescale(slicer.get(), min=-8.0, max = -7.0)
    #psi_soil_sats = rescale(slicer.get(), min=-0.025, max = -0.011) # 1 sigma
    psi_soil_sats = rescale(slicer.get(), min=-0.035, max = -0.007)  # 2 sigmas
    psi_soil_sats *= MPA_TO_HHEAD
    #pore_size_ind = rescale(slicer.get(), min=0.238, max = 0.291) # 1 sigma
    pore_size_ind = rescale(slicer.get(), min=0.216, max = 0.325)  # 2 sigmas

    soil_collection = []
    for i in range(ncombs):
        soil_collection.append(SoilLayer())

    for i in range(ncombs):
        soil_collection[i].k_soil_sat = 10**k_soil_sats_logs[i]
        soil_collection[i].psi_soil_sat = psi_soil_sats[i]
        soil_collection[i].theta_s = 0.6
        soil_collection[i].theta_r = 0.0
        soil_collection[i].pore_size_ind = pore_size_ind[i]

    sel_cols.append("k_soil_sat_01")
    sel_cols.append("psi_soil_sat_01")
    sel_cols.append("theta_s_01")
    sel_cols.append("theta_r_01")
    sel_cols.append("psi_01")


    g0_s            = rescale(slicer.get(), min=0.001, max = 0.02)
    sel_cols.append("g0")
    g1_s            = rescale(slicer.get(), min = 2.4, max = 2.6)
    sel_cols.append("g1")

    anet_max    = rescale(slicer.get(), min = 0.1, max = 1.5)
    sel_cols.append("anet_max")

    k_xylems_sats   = rescale(slicer.get(), min=10, max= 100.0)
    #k_xylems_sats *= KG_TO_MOL
    sel_cols.append("k_xylem_sat")

    huber_values    =  rescale(slicer.get(), min = 1/4500, max= 1/2500)
    sel_cols.append("huber_value")

    cstem_s         = rescale(slicer.get(), min=10, max=600)
    cstem_s *= KG_TO_MOL
    sel_cols.append("kappa_stem")

    # lai_s           = rescale(slicer.get(), min= 4.6, max = 5.0)
    # sel_cols.append("lai")

    cleaf_s_log        = rescale(slicer.get(), min=np.log10(0.0001), max=np.log10(0.01))
    sel_cols.append("kappa_leaf")

    d_50close_s     = rescale(slicer.get(), min = 2.5, max = 4.0)
    sel_cols.append("d50_close")

    psi_50_close_s  = rescale(slicer.get(), min = -2.2, max = -2.1)
    sel_cols.append("psi50_close")

    jackson_s       = rescale(slicer.get(), min = 0.90, max = 0.98)
    sel_cols.append("root_beta")

    psi50_xylems    = rescale(slicer.get(), min = -4.2, max = -3.6)
    sel_cols.append("psi_50_xylem")

    psi88_xylems_offset    = rescale(slicer.get(), min = 1.0, max = 1.5)
    sel_cols.append("psi_88_xylem")

    root_area_indexes    = rescale(slicer.get(), min = 4, max = 8)
    sel_cols.append("root_area_indexes")

    # tree_densities    = rescale(slicer.get(), min = (64-32)/10000, max = (64)/10000)
    # sel_cols.append("tree_densities")

    sigma_wcont    = rescale(slicer.get(), min = -1, max = 0.5)
    sel_cols.append("sigma_wconts")

    soil_profile_indexes = rescale(slicer.get(), min = 0, max = 8).astype(int)

    g_barks    = rescale(slicer.get(), min = 0.005, max = 0.05)
    sel_cols.append("g_barks")

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
        params.k_xylem_sat = k_xylems_sats[i]
        params.leaf_hydraulic_capacitance = (10**cleaf_s_log[i])*KG_TO_MOL
        params.huber_value = huber_values[i]

        params.psi_leaf_50_close = psi_50_close_s[i]
        params.d_50_close = d_50close_s[i]
        params.jackson_root_beta = jackson_s[i]

        #params.wcont_sigma_deviation = sigma_wcont[i]
        params.wcont_sigma_deviation = 0.0

        params.soil_profile_index = soil_profile_indexes[i]

        params.tree_density = 10/1000

        params.canopy_height = 35

        pressure = 1.013 * 100000.0  # Pa
        c_a = 415

        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.stem_flow_type = Stem_Flow_Model_Type.Linear.name
        #params.conductivity_fraction_type = Conductivity_Fraction_Module_Type.Logit

        params.sustain_xylem_damage = True

        params.sw_rad_max = 972.935

        if i % 5000 == 0:
            print(f"{i/ncombs * 100.0}% completed")

        plist.Add(params)

    plist.Write_Full_Parameter_File(f"{parameter_file}{rank}")
    #plist.Write_Partial_Parameter_File(f"{parameter_file}{rank}", sel_cols)


def Calculate_LHS_per_process_swiss_cc_n_one_indiv(rank, ncombs, parameter_fle):
    
    KG_TO_MOL = 1000.0/18.0
    MPA_TO_HHEAD = 1000.0/9.81

    seed   = 12345 * rank + 1321
    sampler = qmc.LatinHypercube(d = 22, seed= seed)
    sample = sampler.random(n = ncombs)
    sample = sample.T

    sel_cols = []
    slicer = Subslicer(array=sample)

    # Values according to David steger ( 50-200 mm day-1)
    k_soil_sats_logs = rescale(slicer.get(), min=-6.2, max = -5.5)
    #psi_soil_sats = rescale(slicer.get(), min=-0.025, max = -0.011) # 1 sigma
    psi_soil_sats = rescale(slicer.get(), min=-0.025, max = -0.011) # 1 sigma
    #psi_soil_sats = rescale(slicer.get(), min=-0.035, max = -0.007)  # 2 sigmas
    psi_soil_sats *= MPA_TO_HHEAD
    pore_size_ind = rescale(slicer.get(), min=0.238, max = 0.291) # 1 sigma
    pore_size_ind = rescale(slicer.get(), min=0.27, max = 0.29) # 1 sigma
    #pore_size_ind = rescale(slicer.get(), min=0.216, max = 0.325)  # 2 sigmas
    #sigma_wcont    = rescale(slicer.get(), min = -1.5, max = 2.0)
    sigma_wcont    = rescale(slicer.get(), min = 0, max = 2.0)

    soil_collection = []
    for i in range(ncombs):
        soil_collection.append(SoilLayer())

    for i in range(ncombs):
        soil_collection[i].k_soil_sat = 10**k_soil_sats_logs[i]
        soil_collection[i].k_soil_sat = 1.5* 10**(-6)
        #soil_collection[i].psi_soil_sat = psi_soil_sats[i]
        soil_collection[i].psi_soil_sat = -0.018 * MPA_TO_HHEAD
        soil_collection[i].theta_s = 0.6
        soil_collection[i].theta_r = 0.0
        #soil_collection[i].pore_size_ind = 0.285
        soil_collection[i].pore_size_ind = pore_size_ind[i]
        soil_collection[i].pore_size_ind = 0.28

    sel_cols.append("k_soil_sat_01")
    sel_cols.append("psi_soil_sat_01")
    sel_cols.append("psi_01")
    sel_cols.append("sigma_wconts")




    g0_s            = rescale(slicer.get(), min= 0.0014, max = 0.0016)
    g1_s            = rescale(slicer.get(), min = 1.75, max = 2.75)
    sel_cols.append("g1")
    
    g_barks_factor    = rescale(slicer.get(), min = 0.0, max = 0.3)
    #k_xylems_sats   = rescale(slicer.get(), min = 0.5, max= 3.0)
    k_xylems_sats   = rescale(slicer.get(), min = 0.3, max= 2.0)
    k_xylems_sats *= KG_TO_MOL
    huber_values    = rescale(slicer.get(), min = 0.0004, max= 0.0006)
    cstem_s         = rescale(slicer.get(), min = 140, max = 200)
    cstem_s *= KG_TO_MOL
    
    cstem_res_s         = rescale(slicer.get(), min = 10, max = 30)
    cstem_res_s *= KG_TO_MOL
    
    psi_tlp_sap_s = rescale(slicer.get(), min = -1.5, max = -2.5)
    
    vmax25s         = rescale(slicer.get(), min = 30, max = 50)
    jmax25s         = rescale(slicer.get(), min = 60, max = 80)

    #k_heart_saps    = rescale(slicer.get(), min = 0.00001, max = 0.0001)


    # lai_s           = rescale(slicer.get(), min= 4.6, max = 5.0)
    cleaf_s_log        = rescale(slicer.get(), min=np.log10(0.005), max=np.log10(0.03))
    d_50close_s     = rescale(slicer.get(), min = 2.5, max = 53.5)
    psi_50_close_s  = rescale(slicer.get(), min = -1.2, max = -0.6)
    #jackson_s       = rescale(slicer.get(), min = 0.90, max = 0.98)
    psi50_xylems    = rescale(slicer.get(), min = -4.2, max = -3.5)
    psi88_xylems_offset    = rescale(slicer.get(), min = 0.3, max = 1.2)
    root_area_indexes    = rescale(slicer.get(), min = 8, max = 12)

    # g0_s            = rescale_mean(slicer.get(), mean = parameters.g0, percent=alpha)
    # sel_cols.append("g0")

    sel_cols.append("g1")
    
    sel_cols.append("vmax25")
    
    sel_cols.append("jmax25")
    
    sel_cols.append("g_stem_res")

    #k_xylems_sats *= KG_TO_MOL
    sel_cols.append("k_xylem_sat")

    sel_cols.append("huber_value")

    #cstem_s *= KG_TO_MOL
    sel_cols.append("kappa_stem")
    
    #cstem_s *= KG_TO_MOL
    sel_cols.append("kappa_stem")


    # lai_s           = rescale(slicer.get(), min= 4.6, max = 5.0)
    # sel_cols.append("lai")
    sel_cols.append("kappa_leaf")

    # d_50close_s     = rescale_mean(slicer.get(), mean = parameters.d_50_close, percent=alpha)
    # sel_cols.append("d50_close")
    
    sel_cols.append("psi50_close")

    jackson_s       = rescale(slicer.get(), min= 0.90, max = 0.99)
    sel_cols.append("root_beta")

    sel_cols.append("psi_50_xylem")

    sel_cols.append("psi_88_xylem")
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

        Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

        params.stem_hydraulic_capacitance = cstem_s[i]
        params.stem_hydraulic_capacitance_res = cstem_s[i]
        params.psi_tlp = psi_tlp_sap_s[i]
        # params.leaf_area_index = lai_s[i]
        params.g0 = g0_s[i]
        #params.g0 = 0.0014
        params.g_stem_res = params.g0* g_barks_factor[i]
        params.g1 = g1_s[i]
        params.vmax25 = vmax25s[i]
        params.jmax25 = jmax25s[i]
        params.jackson_root_beta = 0.99
        
        #params.k_heart_sap = k_heart_saps[i]
        params.k_heart_sap = 0.0

        params.psi50_xylem = psi50_xylems[i]
        params.psi88_xylem = psi50_xylems[i] - psi88_xylems_offset[i]
        params.k_xylem_sat = k_xylems_sats[i]
        params.leaf_hydraulic_capacitance = (10**cleaf_s_log[i])*KG_TO_MOL
        params.huber_value = huber_values[i]
        params.wcont_sigma_deviation = sigma_wcont[i]


        params.psi_leaf_50_close = psi_50_close_s[i]
        params.d_50_close = d_50close_s[i]
        #params.jackson_root_beta = jackson_s[i]

        params.tree_density = 10/1000
        params.canopy_height = 35

        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.stem_flow_type = Stem_Flow_Model_Type.Linear.name
        
        params.conductivity_fraction_type = Conductivity_Fraction_Module_Type.Weibull.name
        
        params.soil_profile_index = 0
        
        params.sustain_xylem_damage = True
        plist.Add(params)
    plist.Write_Full_Parameter_File(f"{parameter_fle}{rank}")
    
    

def Calculate_LHS_per_process_swiss_cc_n_std_n(rank, ncombs, parameter_file):
    
    KG_TO_MOL = 1000.0/18.0
    MPA_TO_HHEAD = 1000.0/9.81

    seed   = 12345 * rank + 1321
    sampler = qmc.LatinHypercube(d = 25, seed= seed)
    sample = sampler.random(n = ncombs)
    sample = sample.T

    sel_cols = []
    slicer = Subslicer(array=sample)

    # Setting up soil layers
    nsoil_layers = 3

    # Values according to David steger ( 50-200 mm day-1)
    k_soil_sats_logs = rescale(slicer.get(), min=-7.0, max = -4.8)
    #psi_soil_sats = rescale(slicer.get(), min=-0.025, max = -0.011) # 1 sigma
    psi_soil_sats = rescale(slicer.get(), min=-0.025, max = -0.011) # 1 sigma
    #psi_soil_sats = rescale(slicer.get(), min=-0.035, max = -0.007)  # 2 sigmas
    psi_soil_sats *= MPA_TO_HHEAD
    pore_size_ind = rescale(slicer.get(), min=0.238, max = 0.291) # 1 sigma
    #pore_size_ind = rescale(slicer.get(), min=0.216, max = 0.325)  # 2 sigmas
    sigma_wcont    = rescale(slicer.get(), min = -1.5, max = 1.5)

    soil_collection = []
    for i in range(ncombs):
        soil_collection.append(SoilLayer())

    for i in range(ncombs):
        soil_collection[i].k_soil_sat = 10**k_soil_sats_logs[i]
        soil_collection[i].psi_soil_sat = psi_soil_sats[i]
        soil_collection[i].theta_s = 0.6
        soil_collection[i].theta_r = 0.0
        soil_collection[i].pore_size_ind = pore_size_ind[i]

    sel_cols.append("k_soil_sat_01")
    sel_cols.append("psi_soil_sat_01")
    sel_cols.append("theta_s_01")
    sel_cols.append("theta_r_01")
    sel_cols.append("psi_01")


    g0_s            = rescale(slicer.get(), min=0.001, max = 0.02)
    sel_cols.append("g0")
    g1_s            = rescale(slicer.get(), min = 2.4, max = 2.6)
    sel_cols.append("g1")

    anet_max    = rescale(slicer.get(), min = 0.1, max = 1.5)
    sel_cols.append("anet_max")

    k_xylems_sats   = rescale(slicer.get(), min=10, max= 100.0)
    #k_xylems_sats *= KG_TO_MOL
    sel_cols.append("k_xylem_sat")

    huber_values    =  rescale(slicer.get(), min = 1/4500, max= 1/2500)
    sel_cols.append("huber_value")

    cstem_s         = rescale(slicer.get(), min=10, max=600)
    cstem_s *= KG_TO_MOL
    sel_cols.append("kappa_stem")

    # lai_s           = rescale(slicer.get(), min= 4.6, max = 5.0)
    # sel_cols.append("lai")

    cleaf_s_log        = rescale(slicer.get(), min=np.log10(0.0001), max=np.log10(0.01))
    sel_cols.append("kappa_leaf")

    d_50close_s     = rescale(slicer.get(), min = 2.5, max = 4.0)
    sel_cols.append("d50_close")

    psi_50_close_s  = rescale(slicer.get(), min = -2.2, max = -2.1)
    sel_cols.append("psi50_close")

    jackson_s       = rescale(slicer.get(), min = 0.90, max = 0.98)
    sel_cols.append("root_beta")

    psi50_xylems    = rescale(slicer.get(), min = -4.2, max = -3.6)
    sel_cols.append("psi_50_xylem")

    psi88_xylems_offset    = rescale(slicer.get(), min = 1.0, max = 1.5)
    sel_cols.append("psi_88_xylem")

    root_area_indexes    = rescale(slicer.get(), min = 4, max = 8)
    sel_cols.append("root_area_indexes")

    # tree_densities    = rescale(slicer.get(), min = (64-32)/10000, max = (64)/10000)
    # sel_cols.append("tree_densities")

    sigma_wcont    = rescale(slicer.get(), min = -1.5, max = 0.5)
    sel_cols.append("sigma_wconts")

    soil_profile_indexes = rescale(slicer.get(), min = 0, max = 8).astype(int)

    g_barks    = rescale(slicer.get(), min = 0.005, max = 0.05)
    sel_cols.append("g_barks")

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
        params.k_xylem_sat = k_xylems_sats[i]
        params.leaf_hydraulic_capacitance = (10**cleaf_s_log[i])*KG_TO_MOL
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

        params.sustain_xylem_damage = True

        params.sw_rad_max = 972.935

        if i % 5000 == 0:
            print(f"{i/ncombs * 100.0}% completed")

        plist.Add(params)

    plist.Write_Full_Parameter_File(f"{parameter_file}{rank}")
    #plist.Write_Partial_Parameter_File(f"{parameter_file}{rank}", sel_cols)
