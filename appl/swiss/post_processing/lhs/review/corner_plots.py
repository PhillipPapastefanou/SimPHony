import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
import seaborn as sns

# --- User Dictionaries and Paths ---
label_dict = {
    'huber_value': r"Huber $\mathcal{H}$",
    'stem_hydraulic_capacitance': r"$\Gamma_\mathrm{sap, max}$",
    'stem_hydraulic_capacitance_res': r"$\Gamma_\mathrm{sap,res}$",
    'k_xylem_sat': r"$K_\mathrm{xyl, max}$",
    'psi88_xylem': r"$\psi_{88}$",
    'wcont_sigma_deviation': r"$\sigma_\mathrm{wcont}$",
    'psi_leaf_50_close': r"$\psi_{Leaf, close}$",
    'p50-p88': r"$\psi_{50} - \psi_{88}$",
    'psi_tlp': r"$\psi_\mathrm{sap, tlp}$"
}

unit_dict = {
    'stem_hydraulic_capacitance': "kg m-3 MPa-1",
    'stem_hydraulic_capacitance_res': "kg m-3 MPa-1",
    'leaf_hydraulic_capacitance': "kg m-2 MPa-1",
    'k_xylem_sat': "kg m-1 s-1 MPa-1",
    'huber_value': "cm2 m-2",
    'psi50_xylem': "MPa",
    'psi88_xylem': "MPa",
    'g0': "mol m-1 s-1",
    'g1': "-",
    'g_stem_res': "mol m-1 s-1",
    'psi_leaf_50_close': "MPa",
    'd_50_close': "-",
    'alpha': "-",
    'gam_stem_x_H': "-",
    'gam_stem_div_g0': "-",
    'k_xyl_x_H': "-",
    'g_min_loss': "-",
    'wcont_sigma_deviation': "-",
    'p50-p88': "MPa",
    'psi_tlp': "MPa",
    'g0_div_gam_stem_x_H': "-",
    'pore_0': "-",
    'k_soil_sat0': "m s-1",
    'root_area_index': "m2 m-2"
}

root_output_directory = "/Net/Groups/BSI/scratch/ppapastefanou/simulations/SimPHony/swiss_cc/"
scenario_name = "45_LHS_TLP"
nmax = 1000

post_path = os.path.join(root_output_directory, scenario_name, 'post')

# ---------------------------------------------------------
# 1. LOAD & CLEAN POSTERIOR DATA
# ---------------------------------------------------------
df = pd.read_csv(os.path.join(post_path, f"All_RMSE_{nmax}.csv"))

df.loc[df['sid'] == 0, 'sid'] = 1 
df.loc[df['sid'] == -1, 'sid'] = 0 
df.loc[df['sid'] == 3, 'sid'] = 2 
df.loc[df['sid'] == 6, 'sid'] = 3 
df.loc[df['sid'] == 7, 'sid'] = 4 

# Calculate derived parameters
df['p50-p88'] = df['psi50_xylem'] - df['psi88_xylem']

# Rescale posterior variables
df['stem_hydraulic_capacitance'] *= 18.0 / 1000.0
df['stem_hydraulic_capacitance_res'] *= 18.0 / 1000.0
df['huber_value'] *= 10000.0
df['k_xylem_sat'] *= 18.0 / 1000.0

# Drop Tree 2 (sid==2) AND Tree 3 (sid==3)
index_to_drop = df[(df['sid'] == 2) | (df['sid'] == 3)].index
df.drop(index_to_drop, inplace=True)

# Keep only Alive (0), Dead Tree 1 (1), and Dead Tree 4 (4)
df_filtered = df[df['sid'].isin([0, 1, 4])].copy()

status_map = {
    0: 'Alive (Average)',
    1: 'Dead Tree 1',
    4: 'Dead Tree 4'
}
df_filtered['Status'] = df_filtered['sid'].map(status_map)

# ---------------------------------------------------------
# 2. LOAD & CLEAN PRIOR DATA 
# ---------------------------------------------------------
prior_filename = os.path.join(post_path, 'parameters_combined_all.csv')
print(f"Loading prior data from {prior_filename}...")
df_prior = pd.read_csv(prior_filename)

# Calculate derived parameters for Prior
df_prior['p50-p88'] = df_prior['psi50_xylem'] - df_prior['psi88_xylem']

# Rescale prior variables exactly like the posterior
df_prior['stem_hydraulic_capacitance'] *= 18.0 / 1000.0
df_prior['stem_hydraulic_capacitance_res'] *= 18.0 / 1000.0
df_prior['huber_value'] *= 10000.0
df_prior['k_xylem_sat'] *= 18.0 / 1000.0

# ---------------------------------------------------------
# 3. IDENTIFIABILITY CALCULATION (PER-TREE AVERAGED)
# ---------------------------------------------------------
target_vars = [
    'wcont_sigma_deviation', 
    'psi_leaf_50_close', 
    'psi88_xylem', 
    'p50-p88', 
    'k_xylem_sat', 
    'huber_value', 
    'stem_hydraulic_capacitance', 
    'psi_tlp', 
    'stem_hydraulic_capacitance_res'
]

# Calculate Prior Standard Deviation
prior_sd_series = df_prior[target_vars].std()

# Calculate Posterior SD *per tree*, then average them to prevent pooling inflation
mean_posterior_sd = df_filtered.groupby('sid')[target_vars].std().mean()

# Calculate Identifiability Ratio
ident_ratios = (mean_posterior_sd / prior_sd_series).sort_values(ascending=True)

print("\n--- Identifiability Ratios (Averaged Per-Tree) ---")
print(ident_ratios)

# Split into two chunks based on rank: top 5 and remaining 4
sorted_params = ident_ratios.index.tolist()
param_chunks = [
    sorted_params[0:5], # First plot (5 variables)
    sorted_params[5:9]  # Second plot (4 variables)
]

# ---------------------------------------------------------
# 4. GENERATE CORNER PLOTS
# ---------------------------------------------------------
sns.set_context("paper", font_scale=1.2)
sns.set_style("ticks")

palette = {
    'Alive (Average)': '#1f77b4',
    'Dead Tree 1': '#d62728',     
    'Dead Tree 4': '#8c564b'      
}

for i, chunk in enumerate(param_chunks):
    num_vars = len(chunk)
    plot_cols = chunk + ['Status']
    
    # Map labels and units
    rename_dict = {}
    for col in chunk:
        label = label_dict.get(col, col)
        unit = unit_dict.get(col, "")
        
        if unit and unit != "-":
            rename_dict[col] = f"{label} [{unit}]"
        else:
            rename_dict[col] = label
            
    df_plot = df_filtered[plot_cols].rename(columns=rename_dict)
    
    print(f"\nGenerating Plot {i+1} with {num_vars} variables: {chunk}")
    
    # Create Pairplot
    g = sns.pairplot(
        df_plot,
        hue='Status',
        palette=palette,
        corner=True, 
        plot_kws={'alpha': 0.5, 's': 15, 'edgecolor': 'none'}, 
        diag_kws={'common_norm': False, 'fill': True, 'alpha': 0.4, 'linewidth': 1.5}
    )
    
    # Annotate the Identifiability Ratio on the diagonal plots
    for idx, col in enumerate(chunk):
        ratio_val = ident_ratios[col]
        ax = g.axes[idx, idx] 
        
        # Format the text with LaTeX notation for R_id
        ax.annotate(f"$R_{{id}} = {ratio_val:.2f}$", 
                    xy=(0.05, 0.95), 
                    xycoords='axes fraction',
                    ha='left', 
                    va='top', 
                    fontsize=10, 
                    bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="gray", alpha=0.8))

    # Dynamic Titles
    title = f"Posterior Distributions (Set {i+1}: Top {num_vars} Ranked by Identifiability)" if i==0 else f"Posterior Distributions (Set {i+1}: Remaining {num_vars} Variables)"
    g.fig.suptitle(title, y=1.02, fontsize=16)
    
    # Save Output
    save_path = os.path.join(post_path, f'posterior_corner_plot_set_{i+1}_({num_vars}vars).pdf')
    plt.savefig(save_path, format="pdf", bbox_inches="tight")
    print(f"Saved: {save_path}")
    
    plt.close()

print("\nAll plots generated successfully.")