import os
import numpy as np
import pandas as pd
import copy

from src.Parameters import Parameters
from src.Parameters import SoilLayer
from src.Parameters import Soil_Water_Model_Type
from src.Parameters import Stem_Flow_Model_Type
from contrib.ParametersList import ParametersList
from src.Parameters import CreateSoilParameters
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


def Calculate_LHS_per_process(rank, ncombs, path):

    # root_output_directory = "/Net/Groups/BSI/work_scratch/ppapastefanou/simulations/plant_hydraulics_standalone/2024/hainich"
    # root_output_directory = "/Users/pp/data/Simulations/A08_Hydraulics_standalone/hainich"
    # scenario_name = "broad_cluster_big"
    # scenario_name = "broad_local_anet_fix"

    KG_TO_MOL = 1000.0/18.0

    seed   = 12345 * rank + 1321
    sampler = qmc.LatinHypercube(d = 24, seed= seed)
    sample = sampler.random(n = ncombs)
    sample = sample.T

    sel_cols = []
    slicer = Subslicer(array=sample)


    # Setting up soil layers
    nsoil_layers = 3

    k_soil_sats_logs = rescale(slicer.get(), min=-7, max = -6)
    psi_soil_sats= rescale(slicer.get(), min=-0.0005, max = -0.002)
    theta_s = rescale(slicer.get(), min=0.47, max = 0.5)
    theta_r = rescale(slicer.get(), min=0.08, max = 0.10)
    pore_size_ind = rescale(slicer.get(), min=0.46, max = 0.49)

    soil_collection = []
    for i in range(ncombs):
        soil_collection.append(SoilLayer())

    for i in range(ncombs):
        soil_collection[i].k_soil_sat = 10**k_soil_sats_logs[i]
        soil_collection[i].psi_soil_sat = psi_soil_sats[i]
        soil_collection[i].theta_s = theta_s[i]
        soil_collection[i].theta_r = theta_r[i]
        soil_collection[i].pore_size_ind = pore_size_ind[i]

    sel_cols.append("k_soil_sat_01")
    sel_cols.append("psi_soil_sat_01")
    sel_cols.append("theta_s_01")
    sel_cols.append("theta_r_01")
    sel_cols.append("psi_01")


    g0_s            = rescale(slicer.get(), min=0.005, max = 0.02)
    sel_cols.append("g0")
    g1_s            = rescale(slicer.get(), min = 1.25, max = 5.0)
    sel_cols.append("g1")

    anet_max    = rescale(slicer.get(), min = 1.0, max = 4.0)
    sel_cols.append("anet_max")

    k_xylems_sats   = rescale(slicer.get(), min=1, max=10)
    k_xylems_sats *= KG_TO_MOL
    sel_cols.append("k_xylem_sat")

    huber_values    =  rescale(slicer.get(), min = 1/4000, max= 1/3000)
    sel_cols.append("huber_value")

    cstem_s         = rescale(slicer.get(), min=50, max=200)
    cstem_s *= KG_TO_MOL
    sel_cols.append("kappa_stem")

    lai_s           = rescale(slicer.get(), min= 4.6, max = 5.0)
    sel_cols.append("lai")

    cleaf_s_log        = rescale(slicer.get(), min=np.log10(0.001), max=np.log10(0.1))
    sel_cols.append("kappa_leaf")

    d_50close_s     = rescale(slicer.get(), min = 1.0, max = 4.0)
    sel_cols.append("d50_close")

    psi_50_close_s  = rescale(slicer.get(), min = -2.4, max = -2.1)
    sel_cols.append("psi50_close")

    jackson_s       = rescale(slicer.get(), min = 0.95, max = 0.97)
    sel_cols.append("root_beta")

    psi50_xylems    = rescale(slicer.get(), min = -3.7, max = -3.4)
    sel_cols.append("psi_50_xylem")

    psi88_xylems_offset    = rescale(slicer.get(), min = 0.6, max = 1.0)
    sel_cols.append("psi_88_xylem")

    root_area_indexes    = rescale(slicer.get(), min = 2, max = 14)
    sel_cols.append("root_area_indexes")

    tree_densities    = rescale(slicer.get(), min = (64-20)/10000, max = (64+20)/10000)
    sel_cols.append("tree_densities")

    plist = ParametersList()
    for i in range(ncombs):
        params = Parameters()

        params.id = i;
        params.root_area_index = root_area_indexes[i]
        soil_layer_top = soil_collection[i]

        soil_layers = []
        # Duplicate the top layer
        soil_layers.append(soil_layer_top)
        soil_layers.append(copy.deepcopy(soil_layer_top))
        soil_layers.append(copy.deepcopy(soil_layer_top))

        soil_layers[0].depth = 0.08
        soil_layers[1].depth = 0.16
        soil_layers[2].depth = 0.32

        CreateSoilParameters(soil_layers=soil_layers, parameters=params)

        params.stem_hydraulic_capacitance = cstem_s[i]
        params.leaf_area_index = lai_s[i]
        params.g0 = g0_s[i]
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

        params.tree_density = tree_densities[i]

        params.canopy_height = 31

        pressure = 1.013 * 100000.0  # Pa
        c_a = 415

        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.stem_flow_type = Stem_Flow_Model_Type.Linear.name

        params.sw_rad_max = 972.935

        if i % 5000 == 0:
            print(i/ncombs * 100.0)

        plist.Add(params)

    plist.Write_Full_Parameter_File(f"{path}/Hainich_parameters.csv{rank}")
    plist.Write_Partial_Parameter_File(f"{path}/Hainich_partial_parameters.csv{rank}", sel_cols)