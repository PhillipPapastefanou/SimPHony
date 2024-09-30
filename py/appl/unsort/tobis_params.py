import numpy as np

def get_default_soil():
    ''' return default soil hydrological properties for cell at -sellonlatbox,9,10,51,52
    '''
    soil_props = {
        'dsoil' :        np.array(5 * [0.50,]), # np.array([0.065, 0.254, 0.913, 2.902, 5.699]), # soil layer thickness [m]
        'wtr_init':      np.array(5 * [0.50,]), # initial relative soil water
        'ice_init':      np.array(5 * [0.00,]), # initial relative soil water
        'porosity':      0.42685,       # volumetric soil porosity [m/m] (i.e. maximum volumetric soil moisture content)
        'field_cap':     0.30442,       # volumetric field capacity [m/m] (i.e. transpiration becomes reduced)
        'wilt_p':        0.11276,       # volumetric plant wilting point [m/m] (i.e. lower limit for transpiration)
        'resi_wtr':      0.033675,      # volumetric residual water content [m/m] (i.e. lower limit for water movement)
        'k_sat':         9.8084e-06,    # saturated hydraulic conductivity [m/s]
        'mpot_sat':      -0.31655,      # saturated soil matrix potential [m]
        'bclapp':        5.4763,        # Clapp & Hornberger exponent
        'pore_size_ind': 0.24985,       # soil pore size index
        'soil_heat_cap': 2.2500e+06,    # heat capacity of soil matrix
        'soil_heat_cnd': 1.6650,        # heat conductivity of soil matrix
        'steepness':     0.1,           # slope [m/m]
        'drmin':         0.001 / (3600 * 1000), # minimum drainage [m/s]
        'drmax':         0.1 / (3600 * 1000),   # maximum drainage [m/s]
        'soilmodel':     'vanGenuchten', # soil hydrology model
        'interpol':      'upstream',     # layer interface interpolation scheme
        'hydroscale':    'point',        # Scale assumptions for soil fluxes
        }
    soil_props['droot'] = soil_props['dsoil'] * np.array([1,1,0.75,0,0]) # rooting depth per layer [m]
    return soil_props