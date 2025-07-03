import pandas as pd
import os
import numpy as np

print("Root is collecting the best output...", end="")
import glob
outpath = "/Net/Groups/BSI/scratch/ppapastefanou/simulations/SimPHony/swiss_cc/02_ex_con_2025_full_gheart/output"
output_files= glob.glob(f"{outpath}/*csv*")
dfcon = pd.DataFrame()
for i in range(0, len(output_files)):
    # for i in range(0,10):
    df = pd.read_csv(os.path.join(outpath, output_files[i]))

    rmse_max = np.max([
        df.loc[0, 'alive_mean'] / 0.48,
        df.loc[1, 't0'] / 0.72,
        df.loc[2, 't3'] / 0.83,
        df.loc[3, 't6'] / 1.28,
        df.loc[4, 't7'] / 1.16]
    )
    df['RMSE_MAX'] = rmse_max
    df['id'] = i
    df['sid'] = np.arange(5)
    df.drop(['Unnamed: 0'], inplace=True, axis=1)
    dfcon = pd.concat([dfcon, df], axis=0)
dfcon.to_csv(os.path.join("/Net/Groups/BSI/scratch/ppapastefanou/simulations/SimPHony/swiss_cc/02_ex_con_2025_full_gheart/post", "aggregated_out.csv"))
print("Done.")