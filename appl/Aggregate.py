import pandas as pd
import xarray as xr
import numpy as np

for i in range(0, 256):
    ds  = xr.open_dataset(f"run/Sens_Output{i}.nc")
    if i == 0:
        df_t = ds.to_pandas()
    else:
        df= ds.to_pandas()
        df_t = pd.concat([df_t, df])
    print(i)

df_t['run_id'] = np.arange(df_t.shape[0])
ds_t = df_t.to_xarray()
ds_t.to_netcdf("Sens_Output_agg.nc")