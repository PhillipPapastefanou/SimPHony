import numpy as np
def get_flow_values(wtr_vol, ice_vol, soil_props, soilmodel='vanGenuchten'):
    ''' Compute hydraulic soil matric potential, conductivity and diffusivity for different soil states and models
        based on the soil layers moisture state (water and ice)
    '''

    porosity, wcres = soil_props['porosity'], soil_props['resi_wtr']
    k_sat, mpot_sat = soil_props['k_sat'], soil_props['mpot_sat']
    bclapp, psi     = soil_props['bclapp'], soil_props['pore_size_ind']

    # poro_eff   = np.maximum(0, porosity - ice_vol) # Compute the effective porosity
    mobile_wtr = np.maximum(0, porosity - wcres)

    ws_rel_resi = np.minimum(1, np.maximum(0, (wtr_vol - wcres) / mobile_wtr))
    ws_rel_zero = np.minimum(1, np.maximum(0, (wtr_vol / porosity)))

    if soilmodel == 'vanGenuchten':
        # Needs to get relative soil water in the bounds between field capacity and saturation
        with np.errstate(divide='ignore'):
            nvgn = psi + 1
            mvgn = psi / nvgn
            mpot = mpot_sat * np.power((np.power(ws_rel_resi,(-1/mvgn)) - 1 ),(1/nvgn))
            cond = k_sat * np.sqrt(ws_rel_resi) * np.square(1 - np.power((1 - np.power(ws_rel_resi,(1/mvgn))),mvgn))
            ws_rel_corr = np.minimum(0.99999, np.maximum(0.00001, ws_rel_resi))
            diff = (-(mpot_sat * bclapp) / mobile_wtr
                    * np.power(ws_rel_corr,(-1/mvgn)) * np.power((1 - np.power(ws_rel_corr,(1/mvgn))),-mvgn)
                    * cond)

    elif soilmodel == 'BrooksCorey':
        # Needs to get relative soil water in the bounds between field capacity and saturation
        with np.errstate(divide='ignore'):
            mpot = mpot_sat * np.power(ws_rel_resi,-bclapp)
            cond = k_sat * np.power(ws_rel_resi,(2 * bclapp + 3))
            diff = -k_sat * mpot_sat * (bclapp / mobile_wtr) * np.power(ws_rel_resi,(bclapp + 2))

    elif soilmodel == 'Campbell':
        # Needs to get relative soil water in the bounds between zero and saturation
        with np.errstate(divide='ignore'):
            mpot = mpot_sat * np.power(ws_rel_zero, -bclapp)
            cond = k_sat * np.power(ws_rel_zero,(2 * bclapp + 3))
            diff = -k_sat * mpot_sat * (bclapp / porosity) * np.power(ws_rel_zero,(bclapp + 2))
    else:
        raise LookupError('Soil model ',soilmodel,'not yet implemented')

    with np.errstate(divide='ignore'):
        ice_impedance = np.minimum(1, np.maximum(0, np.power(10, -6.0 * (ice_vol / mobile_wtr))))

    return mpot, cond * ice_impedance, diff * ice_impedance, ice_impedance