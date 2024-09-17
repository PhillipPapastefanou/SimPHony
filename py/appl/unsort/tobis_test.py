import numpy as np
import matplotlib.pyplot as plt

from tobis_params import get_default_soil
from tobis_soil_models import get_flow_values


soil_props = get_default_soil()

vwc = np.arange(0.0, 0.5, 0.0001)

fig = plt.figure()

ax = fig.add_subplot(111)

mpot_g, cond_g, diff_g, ice_impedance_g = get_flow_values(vwc, 0, soil_props, soilmodel = 'vanGenuchten')
mpot_b, cond_b, diff_b, ice_impedance_b = get_flow_values(vwc, 0, soil_props, soilmodel = 'BrooksCorey')
mpot_c, cond_c, diff_c, ice_impedance_c = get_flow_values(vwc, 0, soil_props, soilmodel = 'Campbell')
plt.plot(vwc, -np.log10(-mpot_g), label = 'vanGenuchten', color = 'red')
plt.plot(vwc, -np.log10(-mpot_b), label = 'BrooksCorey', color = 'blue')
plt.plot(vwc, -np.log10(-mpot_c), label = 'Campbell', color = 'tab:green')

# ax.plot(vwc, np.log10(cond_g), label = 'vanGenuchten')
# ax.plot(vwc, np.log10(cond_b), label = 'BrooksCorey')
# ax.plot(vwc, np.log10(cond_c), label = 'Campbell')

ax.set_ylim((-10, 0))
ax.set_xlabel(r'$\theta$')
ax.set_ylabel(r'$\log_{10}$ hydraulic conductivity [m/s]')
ax.legend()

#plt.show()
plt.savefig('soil models.png')


u = 3