import os
import numpy as np
from src.core.py.Parameters import Parameters
from src.core.py.Parameters import SoilLayer
from src.core.py.Parameters import Soil_Water_Model_Type
from src.core.py.Parameters import Stem_Flow_Model_Type
from src.core.py.Parameters import Convert_Soil_Parameters
from src.contrib.parameter_parser import Parameter_Parser
import copy
from scipy.stats import qmc


def rescale(x, min, max):
    return min + x * (max - min)


def rescale_mean(x, mean, percent):
    min = mean * (100.0 - percent) / 100.0
    max = mean * (100.0 + percent) / 100.0
    return rescale(x, min, max)


class Subslicer:
    def __init__(self, array):
        self.array = array
        self.i = -1

    def get(self):
        self.i += 1
        return self.array[self.i]

def create_example_hainich_file(filename):

    plist = Parameter_Parser()

    # Create parameter setup
    params = Parameters()

    nsoil_layers = 3
    layer = SoilLayer()
    layer.k_soil_sat = 1.0 / 100.0 / 86400.0
    layer.psi_soil_sat = -0.5 * 1
    # layer.camp_b  = 10.4
    layer.theta_s = 0.48
    layer.theta_r = 0.05
    layer.pore_size_ind = 0.6

    # Copy the soil layer and assume all layers have the same properties...
    soil_layers = []
    for s in range(nsoil_layers):
        soil_layers.append(copy.deepcopy(layer))

    # ... but not the depth
    soil_layers[0].depth = 0.08
    soil_layers[1].depth = 0.16
    soil_layers[2].depth = 0.32

    # ------------------------------------------------------
    # Parameter setup
    # ------------------------------------------------------

    Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

    params.canopy_height = 31
    params.huber_value = 1.0 / 3000.0
    params.k_xylem_sat = 5 * 1000 / 18
    params.stem_hydraulic_capacitance = 150 * 1000 / 18
    params.leaf_hydraulic_capacitance = 0.01 * 1000 / 18
    params.g_bark = 0.01
    params.g0 = 0.005
    params.g1 = 1.5
    params.leaf_area_index = 4.8
    params.psi_leaf_50_close = -2.3
    params.d_50_close = 2.0
    params.psi50_xylem = -3.5;
    params.psi88_xylem = -5.5;
    params.root_area_index = 4.5
    params.jackson_root_beta = 0.96
    params.tree_density = 64.0 / 10000.0
    params.anet_max = 2.5
    params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
    params.sw_rad_max = 1040

    plist.Add(params)
    plist.Write_Full_Parameter_File(filename=filename)

def create_example_swiss_cc_file(filename):

    plist = Parameter_Parser()

    # Create parameter setup
    params = Parameters()

    nsoil_layers = 3
    layer = SoilLayer()
    layer.k_soil_sat = 1.0 / 100.0 / 86400.0
    layer.psi_soil_sat = -0.5 * 1
    # layer.camp_b  = 10.4
    layer.theta_s = 0.48
    layer.theta_r = 0.05
    layer.pore_size_ind = 0.6

    # Copy the soil layer and assume all layers have the same properties...
    soil_layers = []
    for s in range(nsoil_layers):
        soil_layers.append(copy.deepcopy(layer))

    # ... but not the depth
    soil_layers[0].depth = 0.1
    soil_layers[1].depth = 0.3
    soil_layers[2].depth = 0.4

    # ------------------------------------------------------
    # Parameter setup
    # ------------------------------------------------------

    Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

    params.canopy_height = 31
    params.huber_value = 1.0 / 3000.0
    params.k_xylem_sat = 5 * 1000 / 18
    params.stem_hydraulic_capacitance = 150 * 1000 / 18
    params.leaf_hydraulic_capacitance = 0.01 * 1000 / 18
    params.g_bark = 0.01
    params.g0 = 0.005
    params.g1 = 1.5
    params.leaf_area_index = 4.8
    params.psi_leaf_50_close = -2.3
    params.d_50_close = 2.0
    params.psi50_xylem = -3.5;
    params.psi88_xylem = -5.5;
    params.root_area_index = 4.5
    params.jackson_root_beta = 0.96
    params.tree_density = 64.0 / 10000.0
    params.anet_max = 2.5
    params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
    params.sw_rad_max = 1040

    plist.Add(params)
    plist.Write_Full_Parameter_File(filename=filename)


def create_example_parameter_list(n, filename):

    ncombs = n

    KG_TO_MOL = 1000.0 / 18.0

    seed = 123456789
    sampler = qmc.LatinHypercube(d=24, seed=seed)
    sample = sampler.random(n=ncombs)
    sample = sample.T

    sel_cols = []
    slicer = Subslicer(array=sample)

    # Setting up soil layers
    nsoil_layers = 3

    k_soil_sats_logs = rescale(slicer.get(), min=-8, max=-6)
    psi_soil_sats = rescale(slicer.get(), min=-0.01, max=-0.001)
    theta_s = rescale(slicer.get(), min=0.45, max=0.5)
    theta_r = rescale(slicer.get(), min=0.05, max=0.12)
    pore_size_ind = rescale(slicer.get(), min=0.4, max=0.6)

    soil_collection = []
    for i in range(ncombs):
        soil_collection.append(SoilLayer())

    for i in range(ncombs):
        soil_collection[i].k_soil_sat = 10 ** k_soil_sats_logs[i]
        soil_collection[i].psi_soil_sat = psi_soil_sats[i]
        soil_collection[i].theta_s = theta_s[i]
        soil_collection[i].theta_r = theta_r[i]
        soil_collection[i].pore_size_ind = pore_size_ind[i]

    sel_cols.append("k_soil_sat_01")
    sel_cols.append("psi_soil_sat_01")
    sel_cols.append("theta_s_01")
    sel_cols.append("theta_r_01")
    sel_cols.append("psi_01")

    g0_s = rescale(slicer.get(), min=0.001, max=0.1)
    sel_cols.append("g0")
    g1_s = rescale(slicer.get(), min=0.5, max=2.5)
    sel_cols.append("g1")

    anet_max = rescale(slicer.get(), min=2, max=4)
    sel_cols.append("anet_max")

    k_xylems_sats = rescale(slicer.get(), min=1, max=20)
    k_xylems_sats *= KG_TO_MOL
    sel_cols.append("k_xylem_sat")

    huber_values = rescale(slicer.get(), min=1 / 6000, max=1 / 3000)
    sel_cols.append("huber_value")

    cstem_s = rescale(slicer.get(), min=20, max=500)
    cstem_s *= KG_TO_MOL
    sel_cols.append("kappa_stem")

    lai_s = rescale(slicer.get(), min=4, max=6)
    sel_cols.append("lai")

    cleaf_s_log = rescale(slicer.get(), min=np.log10(0.001), max=np.log10(1))
    sel_cols.append("kappa_leaf")

    d_50close_s = rescale(slicer.get(), min=1.0, max=5.0)
    sel_cols.append("d50_close")

    psi_50_close_s = rescale(slicer.get(), min=-2.5, max=-2.0)
    sel_cols.append("psi50_close")

    jackson_s = rescale(slicer.get(), min=0.9, max=0.97)
    sel_cols.append("root_beta")

    psi50_xylems = rescale(slicer.get(), min=-3.7, max=-3.4)
    sel_cols.append("psi_50_xylem")

    psi88_xylems_offset = rescale(slicer.get(), min=0.6, max=1.0)
    sel_cols.append("psi_88_xylem")

    root_area_indexes = rescale(slicer.get(), min=2, max=8)
    sel_cols.append("root_area_indexes")

    tree_densities = rescale(slicer.get(), min=90 / 10000, max=150 / 10000)
    sel_cols.append("tree_densities")

    plist = Parameter_Parser()
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

        Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

        params.stem_hydraulic_capacitance = cstem_s[i]
        params.leaf_area_index = lai_s[i]
        params.g0 = g0_s[i]
        params.g1 = g1_s[i]
        params.anet_max = anet_max[i]

        params.psi50_xylem = psi50_xylems[i]
        params.psi88_xylem = psi50_xylems[i] - psi88_xylems_offset[i]
        params.k_xylem_sat = k_xylems_sats[i]
        params.leaf_hydraulic_capacitance = (10 ** cleaf_s_log[i]) * KG_TO_MOL
        params.huber_value = huber_values[i]

        params.psi_leaf_50_close = psi_50_close_s[i]
        params.d_50_close = d_50close_s[i]
        params.jackson_root_beta = jackson_s[i]

        params.tree_density = tree_densities[i]

        params.canopy_height = 31
        params.g_bark = 0.005;

        pressure = 1.013 * 100000.0  # Pa
        c_a = 415

        params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name
        params.stem_flow_type = Stem_Flow_Model_Type.Linear.name

        params.sw_rad_max = 972.935

        if i % 5000 == 0:
            print(i / ncombs * 100.0)

        plist.Add(params)

    plist.Write_Full_Parameter_File(filename)
    #plist.Write_Partial_Parameter_File(f"{path}/Hainich_partial_parameters_{ncombs}.csv", sel_cols)
