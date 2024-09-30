# Importing baseline libraries
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime

# Specify paths so that the PHS model can be imported
root_library_path = '/Users/pp/Documents/Repos/plant_hydro_standalone'
root_data_path = os.path.join(root_library_path, 'data')
cpp_lib_path = os.path.join(root_library_path, 'cpp', 'cmake-build-release')
sys.path.append(cpp_lib_path)

# Importing local libraries and paths
from hydro_standalone import Simulation_Single_Hainich
from hydro_standalone import DateTime

from src.Parameters import Parameters
from src.Parameters import SoilLayer
from src.Parameters import Soil_Water_Model_Type
from src.Parameters import Convert_Soil_Parameters
from contrib.parameter_parser import Parameter_Parser

from appl.Hainich.auxil.output_df import create_output_df
from appl.Hainich.auxil.output_plotter import std_plot
from appl.Hainich.auxil.output_plotter import eval_plot
from appl.Hainich.auxil.output_plotter import eval_plot_24

# Specifying forcing and evalution data paths
forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
sapflux_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
psi_stem_file = os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')

# Reading sapflow and psi_stem data
df_sap_obs = pd.read_csv(sapflux_file)
df_sap_obs['datetime']  = pd.to_datetime(df_sap_obs['datetime'])
df_psi_stem_obs = pd.read_csv(psi_stem_file)
df_psi_stem_obs['time']  = pd.to_datetime(df_psi_stem_obs['time'])
# Convert from kg H2O to mol H2O
df_psi_stem_obs['psi_stem_obs'] = df_psi_stem_obs['FAG']

# ------------------------------------------------------
# Parameter setup
# ------------------------------------------------------


file = '/Users/pp/data/Simulations/A08_Hydraulics_standalone/hainich/vanGenuch_fine/post/best_both_sort.csv'
param_parser = Parameter_Parser()
param_parser.Read_Parameter_List(file)

params = param_parser.parameters_list[0]
soil_layers = param_parser.soil_layers_list[0]

# ----------------------------------------------------------------
# PHS model simulation
# ----------------------------------------------------------------
# Set up the PHS simulation
# read in the parameter file that we just created
sim = Simulation_Single_Hainich()
sim.Init_input(forcing_file, sapflux_file, psi_stem_file)

def LHS_rmse(rt_params):





    Convert_Soil_Parameters(soil_layers=soil_layers, parameters=params)

    # params.canopy_height = 31
    params.k_xylem_sat = rt_params[0]
    params.stem_hydraulic_capacitance = rt_params[1]
    params.leaf_hydraulic_capacitance = rt_params[2]
    # params.g0 = 0.03
    # params.g1 = 1.5
    # params.leaf_area_index = 5.0
    # params.huber_value = 1.0 / 3000.0

    params.anet_max = rt_params[3]
    params.tree_density = rt_params[4]
    #params.psi_leaf_50_close= rt_params[4]
    #params.d_50_close= rt_params[5]
    # params.leaf_area_index = 4.8
    cparameters = params.Create_CParameters(soil_layers=soil_layers)

    # Specify Start and End of the Simulation
    format = "%Y-%m-%d %H:%M:%S"
    date_start_str = "2023-04-01 00:00:00"
    date_end_str = "2023-11-01 00:00:00"
    timestart = DateTime(date_start_str, format)
    timeend   = DateTime(date_end_str, format)

    # Run the simulation
    sim.Init_parameters(cparameters)
    sim.Set_water_pot_initials(-1.0, -0.2)
    sim.Run(timestart, timeend)

    an = sim.Get_analysis()

    # org optimization values
    max_rmse_J = 0.000347
    max_rmse_psi_stem = 0.0668

    # revised optimization values
    max_rmse_J = 0.0002
    max_rmse_psi_stem = 0.0668

    return (an.Get_Rmse_psi_stem()/max_rmse_psi_stem + an.Get_Rmse_J()/max_rmse_J)/2.0


from time import perf_counter
import scipy.optimize as optimize


init= {}
init['k_xylem_sat'] = params.k_xylem_sat
init['c_stem'] = params.stem_hydraulic_capacitance
init['c_leaf'] = params.leaf_hydraulic_capacitance
init['anet_max'] = params.anet_max
# init['psi_leaf_50_close'] = params.psi_leaf_50_close
# init['d_50_close'] = params.d_50_close
init['tree_density'] = params.tree_density

minf = 0.5
maxf = 2.0
bounds = {}
bounds['k_xylem_sat'] = (params.k_xylem_sat * minf, params.k_xylem_sat * maxf)
bounds['c_stem'] =  (params.stem_hydraulic_capacitance * minf, params.stem_hydraulic_capacitance * maxf)
bounds['c_leaf'] = (params.leaf_hydraulic_capacitance * minf, params.leaf_hydraulic_capacitance * maxf)
bounds['anet_max'] = (params.anet_max * minf, params.anet_max * maxf)
# bounds['psi_leaf_50_close'] = (params.psi_leaf_50_close * 1.25, params.psi_leaf_50_close * 0.8)
# bounds['d_50_close'] = (params.d_50_close * minf, params.d_50_close * maxf)
bounds['tree_density'] = (params.tree_density * 0.75, params.tree_density * maxf)

bounds = [(bounds[key][0], bounds[key][1]) for key in bounds]
init = [init[key] for key in init]

t1 = perf_counter()
print(LHS_rmse(init))
t2 = perf_counter()


print(f"time {t2-t1}")


# result = optimize.minimize(LHS_rmse, x0=init, bounds=bounds,
#                            options={'maxiter': 1000}, method='Powell')

result = optimize.dual_annealing(LHS_rmse,
                                 x0=init,
                                 bounds=bounds,
                                 maxiter=100,
                                 seed = 42)
print(init)
print(result.x)

print(LHS_rmse(result.x))
print(result.nfev)
