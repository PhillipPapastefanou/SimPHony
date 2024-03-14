import numpy as np

class Parameters:

    def __init__(self):

        # Root zone depth [m]
        self.root_zone_depth = 0.3
        # Root area index [1]
        # Xu et al: 24
        # Katul et al: 5.5 - 14.2
        self.root_area_index = 24
        # Plant height [m]
        # From Arend et al 2021 SI
        self.canopy_height = 20
        # Density of water [kg m-3]
        self.rho_water = 998
        # Gravitational constant [kg m-1 m-2]
        self.grav = 9.81


        # Viscosity of the leaf to sap flow [1] ??? To be double checked
        self.eta_LS = 1.0 # 1.0 = Water
        # Stem hydraulic capacity [mol m-3 MPa-1]
        # From Meinzer et al. 2011 Figure 13.2
        # Range: 10 - 500 kg m-3 MPa-1
        c_kg = 200
        self.stem_hydraulic_capacitance = c_kg * 1000.0 / 18.0
        # Xylem saturated Hydraulic conductivity [mol m-2 s-1 MPa]
        # from xu et al 2016: 1.7 - 7.5 kg H2O m-2 s-1
        # 1 mole of water equals 18g
        # equals to 93 - 450
        self.k_xylem_sat = 400
        # Huber value [m2 m-2] equals 1/klatosa
        # From sperry et al
        self.huber_value = 1.0/3600.0
        # Xylem water potential at loss of 50% conductivity [MPa]
        self.psi50_xylem = -3.5
        # Slope parameter of xylem water potential curve (has to be postive)[-]
        self.d_50_s = 10.0


        # Leaf hydraulic conductance [mol m-2 MPa]
        # range 0.2 - 1.2 from Blackmann and Brodribb 2011
        self.leaf_hydraulic_capacitance = 1.0
        # Leaf area index [m2 m-2]
        # Data from the swiss site (half hemispherical)
        self.leaf_area_index = 4.8
        # Leaf water potential at which plants close stomates to 50 % [MPa]
        # Something like this can be made up here...
        self.psi_leaf_50_close = -3.0
        # Slope parameter of stomatal closure
        # No reference here yet, be careful with the sign
        self.d_50_close = 10.0

        # Medlyn 2011 model g0 parameter [mol m-2 s-1]
        # (also minmal stomatal condutances)
        # From Medlyn 0 - 0.05
        # From the site 0.05
        self.g0 = 0.005
        # Medlyn 2011 model g1 parameter [1]
        # From Medlynn 1.6 - 12
        self.g1 = 1.5

        # Soil depths
        self.soil_depths = np.repeat(0.1, 11);
        self.soil_depths_arr = np.array2string(self.soil_depths, separator=';')
        self.soil_depths_arr = self.soil_depths_arr[1:-1]
        # Jackson rooting parameter [-]
        self.jackson_root_beta  = 0.96

        # Saturated soil hydraulic conductance [m s-1]
        # Van Looy et al 4.8 - 62 [cm/d]
        k_soil_sat_cm_d = 15.0
        self.k_soil_sat = k_soil_sat_cm_d / 100.0 / 86400.0

        self.theta_r = 0.105
        self.alpha_genucht = 1.0
        self.n_genucht = 6.0
        self.neta_genucht = 0.5

        #Campbell 1974/ Clapp and STuff 1978 shape parameter [1]
        self.camp_b = 10.4
        # Saturation soil water content [1]
        self.theta_s  = 0.426
        # Reference Soil water potential [MPa]
        # from Clapp 1978: -15.3 cm
        self.camp_psi_soil_ref = -1.50042 * 10 ** (-6)


        # Solver precision
        self.solver_precision = 1E-10

        # In seconds
        self.input_step_len = 1800

        # Number of trees per m-2
        self.tree_dens = 1/20.0