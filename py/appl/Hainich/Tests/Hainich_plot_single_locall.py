# Importing baseline libraries
import sys
import os
import numpy as np
import pandas as pd
import datetime
import copy

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
parameters_list = os.path.join(root_library_path, 'py', 'appl',
                               'LHS', 'generator_files', 'Hainich_parameters_1000.csv')




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

# # Convert from micromole H2O m-2 s-1 to mol H2O m-2 s-1
# df_sap_obs['J'] = df_sap_obs['J']/1E3
# # Convert the observed sapflow to math the modelled
# df_sap_obs['J'] *= params.leaf_area_index
# df_sap_obs['J'] *= params.tree_density
# df_sap_obs['J'] *= 1.0/params.huber_value

# ----------------------------------------------------------------
# PHS model simulation
# ----------------------------------------------------------------

file = '/Users/pp/data/Simulations/A08_Hydraulics_standalone/hainich/vanGenuch_fine/post/best_both_sort.csv'
param_parser = Parameter_Parser()
param_parser.Read_Parameter_List(file)

params = param_parser.parameters_list[0]
soil_layers = param_parser.soil_layers_list[0]

for layer in soil_layers:
    layer.theta_s = 5.36890095e-01
    layer.theta_r = 4.32594873e-02
    layer.pore_size_ind = 5.94283964e-01
    layer.k_soil_sat =5.76303310e-08

params.k_xylem_sat = 120
params.tree_density = 3.17062515e-03
params.leaf_hydraulic_capacitance = 6.67059512e-01

cparameters = params.Create_CParameters(soil_layers=soil_layers)


# Set up the PHS simulation
# read in the parameter file that we just created
sim = Simulation_Single_Hainich()
#sim.Init_parameters_fn_single(parameters_list, parameter_id)
sim.Init_parameters(cparameters)
sim.Init_input(forcing_file, sapflux_file, psi_stem_file)
sim.Set_water_pot_initials(-1.0, -0.2)

# Specify Start and End of the Simulation
format = "%Y-%m-%d %H:%M:%S"
date_start_str = "2023-04-01 00:00:00"
date_end_str = "2023-11-01 00:00:00"
timestart = DateTime(date_start_str, format)
timeend   = DateTime(date_end_str, format)

# Run the simulation
sim.Run(timestart, timeend)

# Get output and analysis data
output = sim.Get_output()
an = sim.Get_analysis()

# Create the main output from the analysis
df = create_output_df(output, date_start_str)

# Create standard plots
std_plot(df, '2023-07-09', '2023-07-27', "plt/std_out.png" )
eval_plot(df, df_sap=df_sap_obs, df_psi_stem=df_psi_stem_obs, analysis=an , path="plt/eval_out.png")
eval_plot_24(df, df_sap=df_sap_obs, df_psi_stem=df_psi_stem_obs, analysis=an , path="plt/eval_out_24.png")

