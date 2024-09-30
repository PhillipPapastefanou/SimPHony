import numpy as np
import matplotlib.pyplot as plt
import xarray as xr
import pandas as pd

from scipy import stats
from itertools import combinations

class MCMCAnalysis:
    def __init__(self, input_path, output_path):
        df = xr.open_dataset(f"{input_path}Combs.nc").to_pandas()
        org_cols = df.columns[2:-1]

        print("Going for the standard histograms: ")
        for var in org_cols:
            if var =='k_soil_sat':
                plt.hist(np.log10(df[var]), bins=150, density=True);
            else:
                plt.hist(df[var], bins=150, density=True);
            plt.savefig(f"{output_path}hist{var}.png")
            plt.close()

        print("Going for the coloured histograms: ")
        xi = pd.qcut(df['k_xylem_sat'], 3)
        df['i'] = xi
        s = df['i'].unique()
        for var in df.columns:
            print(var)
            if var == 'i':
                continue
            for si in s:
                dfs = df[df['i'] == si]
                data_fs = dfs[var]
                data = df[var]
                binwidth = (np.max(data) - np.min(data)) / 150
                plt.hist(data_fs, bins=
                np.arange(np.min(data_fs), np.max(data_fs) + binwidth, binwidth),
                         alpha=0.5);
            plt.savefig(f"{output_path}k_xlm_hist{var}.png")
            plt.close()

        print("Going for the correlation: ")
        dA = []
        for colX, colY in list(combinations(org_cols, 2)):
            print(colX)
            R = np.round(stats.spearmanr(df[colX], df[colY]).statistic, 3)
            dA.append([colX, colY, R, R * R])

        df_con = pd.DataFrame(dA, columns=['NameX', "NameY", "R", "R2"])

        df_con.replace({'leaf_hydraulic_capacitance': f'kappaL'}, regex=True, inplace=True)
        df_con.replace({'k_xylem_sat': f'k_xyl'}, regex=True, inplace=True)
        df_con.replace({'leaf_area_index': f'LAI'}, regex=True, inplace=True)
        df_con.replace({'huber_value': f'huber'}, regex=True, inplace=True)
        df_con.replace({'organic_matter_frac': f'om_f'}, regex=True, inplace=True)
        df_con.replace({'jackson_root_beta': f'root_beta'}, regex=True, inplace=True)
        df_con.replace({'k_soil_sat': f'k_soil'}, regex=True, inplace=True)
        df_con.replace({'clay_frac': f'c_f'}, regex=True, inplace=True)
        df_con.replace({'tree_density': 't_dens'}, regex=True, inplace=True)
        df_con.to_csv(f"{output_path}Correlations.csv", index=None)



