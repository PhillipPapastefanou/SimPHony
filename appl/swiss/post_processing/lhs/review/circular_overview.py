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



import matplotlib.pyplot as plt
import numpy as np
from sklearn.preprocessing import MinMaxScaler
import warnings

# Suppress SettingWithCopyWarning from pandas if it pops up
warnings.filterwarnings('ignore')

# 1. Define the 5 axes exactly as we conceptualized them
# Make sure these match the column names in df_norm
axes_cols = [
    'wcont_sigma_deviation',      # Local Water Access
    'psi_leaf_50_close',          # Stomatal Conservatism (higher/less negative = more conservative)
    'huber_value',                # Structural Demand
    'stem_hydraulic_capacitance', # Internal Storage (Sapwood Capacitance)
    'p50-p88'                     # Hydraulic Safety Margin
]

# The display labels for the radar chart (using your LaTeX dictionary styles)
axes_labels = [
    "Local Water Access\n($\sigma_\mathrm{wcont}$)",
    "Stomatal Conservatism\n($\psi_{Leaf, close}$)",
    "Structural Demand\n(Huber $\mathcal{H}$)",
    "Sapwood Capacitance\n($\Gamma_\mathrm{sap, max}$)",
    "Hydraulic Safety Margin\n($\psi_{50} - \psi_{88}$)"
]
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# 1. Set the global style (Pure white background)
sns.set_theme(style="ticks", context="paper")
plt.rcParams.update({
    "figure.facecolor": "white",
    "axes.facecolor": "white",
    "axes.spines.top": False,  # Removes top border
    "axes.spines.right": False # Removes right border
})

# Create the figure
fig, ax = plt.subplots(figsize=(6, 4), dpi=300)

# 2. Draw the Violins with premium aesthetics
sns.violinplot(
    data=df, 
    x='group', 
    y='parameter_value',
    palette=["#2b8cbe", "#de2d26", "#fcae91", "#fcae91", "#fcae91"], # Professional, high-contrast colors
    inner=None,      # Removes default messy inner lines
    linewidth=1.5,   # Makes the outer border of the violin crisp and distinct
    cut=0,           # Trims the violin ends exactly at the data limits (stops the "stretching" effect)
    density_norm='width', # Ensures all violins have the same maximum width for clean alignment
    ax=ax
)

# 3. Draw the thick mean lines manually (recreating your original look, but cleaner)
# Assuming 'df' is your DataFrame
groups = df['group'].unique()
for i, group_name in enumerate(groups):
    # Calculate the mean for the group
    mean_val = df[df['group'] == group_name]['parameter_value'].mean()
    
    # Draw a thick, crisp horizontal line for the mean
    ax.hlines(y=mean_val, xmin=i - 0.2, xmax=i + 0.2, color='black', linewidth=2.5, zorder=3)
    
    # Add the text label slightly to the right of the mean line
    ax.text(i + 0.25, mean_val, f'{mean_val:.2f}', va='center', ha='left', fontsize=10, color='black')

# Clean up axes labels
ax.set_ylabel("Your Parameter Unit", fontsize=12)
ax.set_xlabel("") # Remove the x-axis label to reduce clutter
plt.tight_layout()
# 2. Calculate the Median profiles for Surviving (alive == 1) and Dying (alive == 0)
df_surviving = df_norm[df_norm['alive'] == 1][axes_cols].median()
df_dying = df_norm[df_norm['alive'] == 0][axes_cols].median()

# 3. Normalize the data to a 0 to 1 scale so they fit on the same radar plot
scaler = MinMaxScaler()
# Fit the scaler on the entire dataset to establish the absolute Min (0) and Max (1)
scaler.fit(df_norm[axes_cols])

# Transform the medians into the 0-1 scale
surviving_norm = scaler.transform([df_surviving])[0].tolist()
dying_norm = scaler.transform([df_dying])[0].tolist()

# Close the polygon loop by appending the first value to the end of the lists
surviving_norm += [surviving_norm[0]]
dying_norm += [dying_norm[0]]

# Calculate the angle for each axis (divide the circle by 5)
angles = [n / float(len(axes_cols)) * 2 * np.pi for n in range(len(axes_cols))]
angles += [angles[0]]

# 4. Initialize the matplotlib polar plot
fig, ax = plt.subplots(figsize=(8, 8), subplot_kw=dict(polar=True))

# Rotate the plot so the first axis is at the very top, and draw clockwise
ax.set_theta_offset(np.pi / 2)
ax.set_theta_direction(-1)

# Draw one axe per variable and add labels
plt.xticks(angles[:-1], axes_labels, color='black', size=11, fontweight='bold')

# Remove the radial (y-axis) tick labels to keep it clean (the 0 to 1 scale)
ax.set_yticklabels([])
# Optional: Set the radial limits strictly to 0-1 so the polygon fills the space properly
ax.set_ylim(0, 1)

# 5. Plot the Blue Polygon (Surviving)
ax.plot(angles, surviving_norm, color='tab:blue', linewidth=2.5, linestyle='solid', label='Surviving Configuration')
ax.fill(angles, surviving_norm, color='tab:blue', alpha=0.3)

# 6. Plot the Red Polygon (Dying)
ax.plot(angles, dying_norm, color='tab:red', linewidth=2.5, linestyle='solid', label='Dying Configuration')
ax.fill(angles, dying_norm, color='tab:red', alpha=0.3)

# Add a legend and title
plt.legend(loc='upper right', bbox_to_anchor=(1.3, 1.1), fontsize=11)
plt.title("Divergent Emergent Configurations Under Drought", size=14, fontweight='bold', y=1.1)

plt.tight_layout()
#plt.show()

# Optional: Save as a high-res PDF for the manuscript
p = os.path.join(post_path, "Figure_Radar_Plot.pdf")
plt.savefig(p, format='pdf', dpi=300, bbox_inches='tight')