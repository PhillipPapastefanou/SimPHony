import numpy as np
import matplotlib.pyplot as plt

from tobis_params import get_default_soil
from tobis_soil_models import get_flow_values


soil_props = get_default_soil()

vwc = np.asarray([0.05, 0.35])

fig = plt.figure()

ax = fig.add_subplot(111)

mpot_g, cond_g, diff_g, ice_impedance_g = get_flow_values(vwc, 0, soil_props, soilmodel = 'vanGenuchten')
mpot_b, cond_b, diff_b, ice_impedance_b = get_flow_values(vwc, 0, soil_props, soilmodel = 'BrooksCorey')
mpot_c, cond_c, diff_c, ice_impedance_c = get_flow_values(vwc, 0, soil_props, soilmodel = 'Campbell')

x = 3;