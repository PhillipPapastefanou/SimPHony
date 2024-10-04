import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df =pd.read_csv("../../../data/swiss/Water_content_MultiSoils.csv")
df.set_index("dates", inplace=True)
ds = pd.DataFrame()

ds['q50']=df.iloc[:,np.arange(0,24,3)].median(axis=1)
ds['q25']=df.iloc[:,np.arange(0,24,3)].quantile(axis=1, q=0.25)
ds['q10']=df.iloc[:,np.arange(0,24,3)].quantile(axis=1, q=0.10)
ds['q75']=df.iloc[:,np.arange(0,24,3)].quantile(axis=1, q=0.75)
ds['q90']=df.iloc[:,np.arange(0,24,3)].quantile(axis=1, q=0.9)