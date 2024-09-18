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
from src.Parameters import CreateSoilParameters
from contrib.ParametersList import ParametersList

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
# Create parameter setup
params = Parameters()

# Define water retention curve model
params.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name

params.anet_max = 2.78 * 0.7
params.sw_rad_max = 972.935
params.root_area_index = 11.150282
params.canopy_height = 31
#params.stem_flow_type =
params.stem_hydraulic_capacitance = 8531.092132 * 1
params.k_xylem_sat = 834.700738 * 0.1
params.huber_value = 0.000325 * 0.8
params.psi50_xylem = -3.579055
params.psi88_xylem = -4.498665
params.leaf_hydraulic_capacitance = 0.129361  *5.0
params.leaf_area_index = 5.812166
params.psi_leaf_50_close = -2.40
params.d_50_close = 3.5030428
params.g0 = 0.015116*1 * 1.5
params.g1 = 1.303983*4
params.jackson_root_beta = 0.935248
params.tree_density = 0.005412*1.5

nsoil_layers = 3
layer = SoilLayer()
layer.k_soil_sat = 3.267113e-07 * 1.0 * 10
layer.psi_soil_sat = -0.02828579 * 12
#layer.camp_b  = 10.4
layer.theta_s = 0.49257
layer.theta_r = 0.059184 * 1.8
layer.pore_size_ind = 0.558836 * 0.6


# Copy the soil layer and assume all layers have the same properties...
soil_layers = []
for s in range(nsoil_layers):
    soil_layers.append(copy.deepcopy(layer))

# ... but not the depth
soil_layers[0].depth = 0.08
soil_layers[1].depth = 0.16
soil_layers[2].depth = 0.32

CreateSoilParameters(soil_layers=soil_layers, parameters=params)



# # Convert from micromole H2O m-2 s-1 to mol H2O m-2 s-1
# df_sap_obs['J'] = df_sap_obs['J']/1E3
# # Convert the observed sapflow to math the modelled
# df_sap_obs['J'] *= params.leaf_area_index
# df_sap_obs['J'] *= params.tree_density
# df_sap_obs['J'] *= 1.0/params.huber_value
# Generate a parameterlist...
plist = ParametersList()
plist.Add(params)
parameters_list = "HainichParameterList1.csv"
# ... and write the one parameter setting.
plist.Write_Full_Parameter_File(parameters_list)

# ----------------------------------------------------------------
# PHS model simulation
# ----------------------------------------------------------------
# Set up the PHS simulation
# read in the parameter file that we just created
sim = Simulation_Single_Hainich()
sim.Init_parameters_fn_single(parameters_list, 0)
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
std_plot(df, '2023-07-09', '2023-07-27', "out/std_out.png" )
std_plot(df, '2023-07-01', '2023-09-01', "out/std_out_all.png" )
eval_plot(df, df_sap=df_sap_obs, df_psi_stem=df_psi_stem_obs, analysis=an , path="out/eval_out.png")
eval_plot_24(df, df_sap=df_sap_obs, df_psi_stem=df_psi_stem_obs, analysis=an , path="out/eval_out_24.png")

print("Average Transpiration per day per tree with 80 m^2 crownwater: " +
      str(df['T'].mean() * 18/1000.0 * 3600.0 *24.0  * params.leaf_area_index * 80) + " liters of water.")
