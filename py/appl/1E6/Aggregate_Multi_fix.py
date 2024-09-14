import pandas as pd
import xarray as xr
import numpy as np

ds_list =  []
si = 0
for i in range(0, 64):
    dh = xr.open_dataset(f"Sens_Output{i}.nc")

    size = dh['run_id'].shape[0]

    dh['run_id'] = np.arange(si, si+size)


    ds_list.append(dh)
    si += size
    print(f"{i} {si}")

ds_t = xr.concat(ds_list, dim='run_id')
ds_t.to_netcdf("Sens_Output_agg.nc")

