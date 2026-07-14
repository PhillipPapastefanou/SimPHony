import pandas as pd
import numpy as np
import xarray as xr

import matplotlib.pyplot as plt
import os
from pathlib import Path
import seaborn as sns
from scipy.stats import wasserstein_distance
from matplotlib.pyplot import tight_layout

from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier, RandomForestRegressor
from sklearn.inspection import permutation_importance

label_dict  = {}
label_dict['huber_value'] = r"Huber $\mathcal{H}$"
label_dict['stem_hydraulic_capacitance'] = r"$\Gamma_\mathrm{sap, max}$"
label_dict['stem_hydraulic_capacitance_res'] = r"$\Gamma_\mathrm{sap,res}$"
label_dict['leaf_hydraulic_capacitance'] = r"$\Gamma_\mathrm{Leaf}$"
label_dict['g0'] = r"$g_0$"
label_dict['g1'] = r"$g_1$"
label_dict['k_xylem_sat'] = r"$K_\mathrm{xyl, max}$"
label_dict['psi50_xylem'] = r"$\psi_{50}$"
label_dict['psi88_xylem'] = r"$\psi_{88}$"
label_dict['wcont_sigma_deviation'] = r"$\sigma_\mathrm{wcont}$"
label_dict['pore_0'] = r"$\xi$"
label_dict['jackson_root_beta'] = r"$\mathrm{Root}_\beta$"
label_dict['psi_leaf_50_close'] = r"$\psi_{Leaf, close}$"
label_dict['d_50_close'] = r"$d_{Leaf, close}$"
label_dict['alpha'] = r"$\alpha$"
label_dict['gam_stem_x_H'] = r"$\Gamma_\mathrm{Stem} \cdot \mathcal{H}$"
label_dict['gam_stem_div_g0'] = r"$\frac{g_0}{\Gamma_\mathrm{Stem}}$"
label_dict['k_xyl_x_H'] = r"$K_\mathrm{xyl, max}\cdot \mathcal{H}$"
label_dict['g_min_loss'] = r"$g_0 + g_\mathrm{Bark}$"
label_dict['g_min_loss'] = r"$g_0 + g_\mathrm{Bark}$"
label_dict['wcont_sigma_deviation'] = r"$\sigma_\mathrm{wcont}$"
label_dict['p50-p88'] =  r"$\psi_{50} - \psi_{88} $"
label_dict['g0_div_gam_stem_x_H'] =  r"g0_div_gam_stem_x_H"
label_dict['g0_div_gam_stem_x_H'] =  r"g0_div_gam_stem_x_H"
label_dict['k_soil_sat0'] =  r"k_soil_sat"
label_dict['root_area_index'] =  r"RAI"
label_dict['psi_tlp'] = r'$\psi_\mathrm{sap, tlp}$'
label_dict['g_stem_res'] = r"$g_\mathrm{stem,res}$"



unit_dict= {}
unit_dict['stem_hydraulic_capacitance'] = "kg m-3 MPa-1"
unit_dict['stem_hydraulic_capacitance_res'] = "kg m-3 MPa-1"
unit_dict['leaf_hydraulic_capacitance'] = "kg m-2 MPa-1"
unit_dict['k_xylem_sat'] = "kg m-1 s-1 MPa-1"
unit_dict['huber_value'] = "cm2 m-2"
unit_dict['psi50_xylem'] = "MPa"
unit_dict['psi88_xylem'] = "MPa"
unit_dict['g0'] = "mol m-1 s-1"
unit_dict['g1'] = "-"
unit_dict['g_stem_res'] = "mol m-1 s-1"
unit_dict['psi_leaf_50_close'] = r"MPa"
unit_dict['d_50_close'] = "-"
unit_dict['alpha'] = r"-"
unit_dict['gam_stem_x_H'] = r"-"
unit_dict['gam_stem_div_g0'] = r"-"
unit_dict['k_xyl_x_H'] =r"-"
unit_dict['g_min_loss'] = r"-"
unit_dict['wcont_sigma_deviation'] = r"-"
unit_dict['p50-p88'] =  "MPa"
unit_dict['psi_tlp'] =  "MPa"
unit_dict['g0_div_gam_stem_x_H'] =  "-"
unit_dict['pore_0'] =  "-"
unit_dict['k_soil_sat0'] =  "m s-1"
unit_dict['root_area_index'] =  "m2 m-2"


root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss/excon_other_soil_other_cstem"
scenario_name = "big_overview_20_10000p"

root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss/excon_2024_final"
scenario_name = "big_overview_20_10000p_fine_g1fix_relay"

root_output_directory = "/Net/Groups/BSI/scratch/ppapastefanou/simulations/SimPHony/swiss_cc/"
#scenario_name = "45-2_LHS_high_wcontvar_full"
scenario_name = "45-3_LHS_no_gstem_full"

root_output_directory = "/Net/Groups/BSI/work_scratch/ppapastefanou/"
scenario_name = "45_LHS_TLP"

nmax = 1000
#scenario_name = "23_ex_con_2025_full_logit_mean_one_wcont"

input_path = os.path.join(root_output_directory, scenario_name, 'input')
output_path = os.path.join(root_output_directory, scenario_name, 'output')
post_path = os.path.join(root_output_directory, scenario_name, 'post')
vars_all = ['psi_leaf_50_close', 'd_50_close', 'stem_hydraulic_capacitance', 'k_xylem_sat', 'huber_value',
        'leaf_hydraulic_capacitance', 'psi50_xylem', 'psi88_xylem', 'g0', 'k_soil_sat0', 'psi_soil_sat0', 'jackson_root_beta',
        'pore_0', 'root_area_index', 'wcont_sigma_deviation', 'alpha', 'g_stem_res','gam_stem_x_H','k_xyl_x_H', 
        'g0_div_gam_stem_x_H', 'g_min_loss', 'p50-p88', 'g1', 'psi_tlp']

vars_all.append('stem_hydraulic_capacitance_res')

vars_plant = ['stem_hydraulic_capacitance', 'k_xylem_sat', 'huber_value', 'leaf_hydraulic_capacitance', 'psi50_xylem',
              'g0','psi_leaf_50_close', 'd_50_close', 'alpha','psi88_xylem']

vars_plant_ex = ['stem_hydraulic_capacitance', 'k_xylem_sat', 'huber_value', 'leaf_hydraulic_capacitance', 'psi50_xylem',
              'g0','psi_leaf_50_close', 'd_50_close', 'alpha','psi88_xylem', 'gam_stem_x_H','k_xyl_x_H', 'g0_div_gam_stem_x_H', 'g_min_loss']
vars_all_ex = vars_all.copy()
vars_all_ex.append("RMSE_MAX")

df = pd.read_csv(os.path.join(post_path, f"All_RMSE_{nmax}.csv"))
df['stem_hydraulic_capacitance'] *= 18.0/1000.0
df['stem_hydraulic_capacitance_res'] *= 18.0/1000.0
df['huber_value'] *= 10000.0
df['k_xylem_sat'] *= 18.0/1000.0
df['alpha'] = df['g_stem_res']/ df['g0']
df['gam_stem_x_H'] = df['stem_hydraulic_capacitance'] * df['huber_value']
df['k_xyl_x_H'] = df['k_xylem_sat'] * df['huber_value']
df['g0_div_gam_stem_x_H'] =  df['g0']/df['stem_hydraulic_capacitance']* df['huber_value']
df['g_min_loss'] = df['g0'] + df['g_stem_res']
df['p50-p88'] = df['psi50_xylem'] - df['psi88_xylem']


df.loc[df['sid'] ==0, 'sid']  =1 
df.loc[df['sid'] ==-1, 'sid'] =0 
df.loc[df['sid'] ==3, 'sid'] = 2 
df.loc[df['sid'] ==6, 'sid'] = 3 
df.loc[df['sid'] ==7, 'sid'] = 4 

index_to_drop = df[(df['sid']==2)|(df['sid']==2)].index
df.drop(index_to_drop, inplace=True)

dv = df[vars_all]
dv['leaf_hydraulic_capacitance'] = np.log10(dv['leaf_hydraulic_capacitance'])
df_norm = dv
df_norm['sid'] = df['sid']


df_norm['alive'] = 1
df_norm.loc[df_norm['sid'] > 0, 'alive'] = 0

df_norm["sid_group"] = df_norm["sid"].apply(lambda s: "sid 0" if s == 0 else "other")
palette = {"sid 0": "tab:blue", "other": "tab:red"}
import plotly.graph_objects as go
from sklearn.preprocessing import MinMaxScaler

# 1. Define your axes (same as before)
axes_cols = [
    'wcont_sigma_deviation', 
    'psi_leaf_50_close', 
    'huber_value', 
    'stem_hydraulic_capacitance', 
    'p50-p88'
]

axes_labels = [
    "Local Water Access",
    "Stomatal Conservatism",
    "Structural Demand",
    "Sapwood Capacitance",
    "Hydraulic Safety Margin"
]

# 2. Calculate Medians (assuming df_norm is already loaded from your script)
df_surviving = df_norm[df_norm['alive'] == 1][axes_cols].median()
df_dying = df_norm[df_norm['alive'] == 0][axes_cols].median()

# 3. Normalize to a 0-1 scale
scaler = MinMaxScaler()
scaler.fit(df_norm[axes_cols])

# Transform and close the loop for Plotly (append first value to the end)
surviving_norm = scaler.transform([df_surviving])[0].tolist()
surviving_norm += [surviving_norm[0]]

dying_norm = scaler.transform([df_dying])[0].tolist()
dying_norm += [dying_norm[0]]

# Plotly also needs the labels list to loop
axes_labels += [axes_labels[0]]

# 4. Build the Plotly Figure
fig = go.Figure()

# Add Surviving Polygon
fig.add_trace(go.Scatterpolar(
      r=surviving_norm,
      theta=axes_labels,
      fill='toself',
      name='Surviving Configuration',
      line_color='blue'
))

# Add Dying Polygon
fig.add_trace(go.Scatterpolar(
      r=dying_norm,
      theta=axes_labels,
      fill='toself',
      name='Dying Configuration',
      line_color='red'
))

# 5. Update Layout for Publication Quality
fig.update_layout(
  polar=dict(
    radialaxis=dict(
      visible=True,
      range=[0, 1],       # Locks the scale from 0 to 1
      showticklabels=False # Hides the 0.2, 0.4 numbers to keep it clean
    )),
  showlegend=True,
  title="Divergent Emergent Configurations Under Drought"
)

# Show the interactive plot in your notebook/browser
p = os.path.join(post_path, "Figure_Radar_Plot2.pdf")
fig.write_image(p, width=800, height=800)

# To save for your manuscript (requires the 'kaleido' package: pip install -U kaleido)
# fig.write_image("Figure_Radar_Plot.pdf", width=800, height=800)