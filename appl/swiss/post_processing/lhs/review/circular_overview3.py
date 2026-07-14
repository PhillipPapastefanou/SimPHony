import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.preprocessing import MinMaxScaler
import warnings

# Suppress warnings for clean console output
warnings.filterwarnings('ignore')

# ==========================================
# 1. GLOBAL PUBLICATION STYLE SETTINGS
# ==========================================
# Pure white background and clean typography globally
sns.set_theme(style="ticks", context="paper")
plt.rcParams.update({
    "figure.facecolor": "white",
    "axes.facecolor": "white",
    "font.size": 12,
    "axes.titlesize": 15,
    "legend.frameon": False,
    "legend.fontsize": 11
})

# ==========================================
# 2. FILE PATHS & DATA LOADING
# ==========================================
# Update these paths to match your local machine
root_output_directory = "/Net/Groups/BSI/work_scratch/ppapastefanou/"
scenario_name = "45_LHS_TLP"
nmax = 1000
post_path = os.path.join(root_output_directory, scenario_name, 'post')
file_path = os.path.join(post_path, f"All_RMSE_{nmax}.csv")

# Load data (assuming it exists here)
# If testing without the file, you can comment out the read_csv and use your existing df
df = pd.read_csv(file_path)

# ==========================================
# 3. DATA TRANSFORMATION & SCALING
# ==========================================
# Apply your specific unit conversions
df['stem_hydraulic_capacitance'] *= 18.0/1000.0
df['huber_value'] *= 10000.0
df['p50-p88'] = df['psi50_xylem'] - df['psi88_xylem']

# Map sid to 'alive' binary status (1 = Surviving, 0 = Dying)
df.loc[df['sid'] == 0, 'sid'] = 1
df.loc[df['sid'] == -1, 'sid'] = 0
df.loc[df['sid'] == 3, 'sid'] = 2
df.loc[df['sid'] == 6, 'sid'] = 3
df.loc[df['sid'] == 7, 'sid'] = 4

# Drop sid 2 as in your original script
index_to_drop = df[(df['sid'] == 2)].index
df.drop(index_to_drop, inplace=True)

df['alive'] = 1
df.loc[df['sid'] > 0, 'alive'] = 0

# Define the 6 axes (adding K_xylem_sat)
axes_cols = [
    'wcont_sigma_deviation',        # Local Water Access
    'psi_leaf_50_close',            # Stomatal Regulation 
    'huber_value',                  # Hydraulic Efficiency 1
    'k_xylem_sat',                  # Hydraulic Efficiency 2
    'stem_hydraulic_capacitance',   # Sapwood Capacitance
    'p50-p88'                       # Hydraulic Safety Margin
]

# Display labels with the grouped "Hydraulic Efficiency" titles
axes_labels = [
    "Local Water Access\n($\sigma_\mathrm{wcont}$)",
    "Stomatal Regulation\n($\psi_{Leaf, close}$)",
    "Structure\n(Huber $\mathcal{H}$)",          # Shortened here
    "Transport\n($K_\mathrm{xyl, max}$)",        # Shortened here
    "Sapwood Capacitance\n($\Gamma_\mathrm{sap, max}$)",
    "Hydraulic Safety Margin\n($\psi_{50} - \psi_{88}$)"
]
# Calculate the Medians for Surviving and Dying groups
df_surviving = df[df['alive'] == 1][axes_cols].median()
df_dying = df[df['alive'] == 0][axes_cols].median()

# Normalize the data to a 0-1 scale so all axes fit on the same polar plot
scaler = MinMaxScaler()
scaler.fit(df[axes_cols])

surviving_norm = scaler.transform([df_surviving])[0].tolist()
dying_norm = scaler.transform([df_dying])[0].tolist()

# Close the polygon loop (connect the last point back to the first point)
surviving_norm += [surviving_norm[0]]
dying_norm += [dying_norm[0]]

# Calculate angles for each of the 5 axes
angles = [n / float(len(axes_cols)) * 2 * np.pi for n in range(len(axes_cols))]
angles += [angles[0]]

# ==========================================
# 4. PLOTTING THE RADAR CHART
# ==========================================
fig, ax = plt.subplots(figsize=(8, 8), subplot_kw=dict(polar=True), dpi=300)

# Rotate so the first axis (Water Access) is pointing straight up
ax.set_theta_offset(np.pi / 2)
ax.set_theta_direction(-1)

# Format the grid and axes
ax.spines['polar'].set_visible(False)            # Remove the outer border circle
ax.grid(color='#D3D3D3', linestyle='--', lw=1)   # Light, clean inner web grid

# Set the axis labels
plt.xticks(angles[:-1], axes_labels, color='black', size=12, fontweight='bold')


mid_angle = (angles[2] + angles[3]) / 2

# Place the text slightly further out than the standard labels (radius 1.25 to 1.35)
ax.text(
    mid_angle, 1.0, 
    "Hydraulic Efficiency", 
    color='black', 
    size=13, 
    fontweight='bold', 
    ha='center', 
    va='center'
)

# You may need to slightly adjust the radial (y-axis) limit so the new text isn't cut off
ax.set_ylim(0, 1.15)

# Remove radial (y-axis) ticks to keep the plot clean (it's 0-1 normalized anyway)
ax.set_yticklabels([])
ax.set_ylim(0, 1.05)  # Slight buffer at the outer edge

# Draw Blue Polygon (Surviving Configuration)
ax.plot(angles, surviving_norm, color='#2b8cbe', linewidth=2.5, linestyle='solid', label='Surviving trees')
ax.fill(angles, surviving_norm, color='#2b8cbe', alpha=0.25)

# Draw Red Polygon (Dying Configuration)
ax.plot(angles, dying_norm, color='#de2d26', linewidth=2.5, linestyle='solid', label='Dying trees')
ax.fill(angles, dying_norm, color='#de2d26', alpha=0.25)

# Add Legend and Title
plt.legend(loc='upper right', bbox_to_anchor=(1.35, 1.05))
#plt.title("Divergent Emergent Configurations Under Drought", size=16, fontweight='bold', y=1.12)

# Adjust layout to prevent text clipping
plt.tight_layout()

# Save as publication-ready PDF and PNG
save_path_pdf = os.path.join(post_path, "Figure_23Radar_Plot.pdf")
save_path_png = os.path.join(post_path, "Figure_23Radar_Plot.png")

plt.savefig(save_path_pdf, format='pdf', dpi=300, bbox_inches='tight')
plt.savefig(save_path_png, format='png', dpi=300, bbox_inches='tight')

print(f"Radar chart saved successfully to {post_path}")