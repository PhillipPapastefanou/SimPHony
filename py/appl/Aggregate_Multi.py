import pandas as pd
import xarray as xr
import numpy as np

ds_list =  []
for i in range(0, 8):
    ds_list.append(xr.open_dataset(f"Sens_Output{i}.nc"))
    print(i)
	
ds_t = xr.concat(ds_list, dim='run_id')
ds_t.to_netcdf("Sens_Output_agg.nc")
