import numpy as np
from enum import Enum

class Conductivity_Fraction_Module_Type(Enum):
    Weibull = 0
    Logit = 1

class Soil_Water_Model_Type(Enum):
    VanGenuchten    = 0
    Saxton06        = 1
    Campbell        = 2

class Stem_Flow_Model_Type(Enum):
    Linear                  = 0
    KirchhoffWeibull        = 1
    KirchhoffPiecewiseErf   = 2


class Parameters:

    def __init__(self):
        # Universal parameter id
        self.id = -1

        # Simulation timestep length [s]
        self.dts = 1800.0

        # In seconds
        self.dts_input = 1800


        # Extended shell output
        self.verbose = False
        # Maximum leaf water potential change per hour [MPa]
        self.max_psi_leaf_change_per_hour = 1.0;
        # Multiplier to estimate the minimum leaf water potential
        # psi_leaf_min = minimum_psi_leaf_multiplier x psi_88
        self.minimum_psi_leaf_multiplier = 4.0;


        self.conductivity_fraction_type = Conductivity_Fraction_Module_Type.Weibull.name

        self.sustain_xylem_damage = False


        # Root area index [1]
        # Xu et al: 24
        # Katul et al: 5.5 - 14.2
        self.root_area_index = 24
        # Plant height [m]
        # From Arend et al 2021 SI
        self.canopy_height = 20

        # Stemflow type module
        self.stem_flow_type = Stem_Flow_Model_Type.Linear.name

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
        # Xylem water potential at loss of 50% conductivity [MPa]
        self.psi88_xylem = -6.0

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

        self.soil_water_model_type =  Soil_Water_Model_Type.VanGenuchten.name

        # Soil depths
        self.soil_depths = np.repeat(0.1, 3);
        self.soil_depths = np.array2string(self.soil_depths, separator=';')
        self.soil_depths = self.soil_depths[1:-1]


        # Jackson rooting parameter [-]
        self.jackson_root_beta  = 0.96

        # Saturated soil hydraulic conductance [m s-1]
        self.k_soil_sats = self.array_to_list_entry(np.array([1.0 / 100.0 / 86400.0, 1.0 / 100.0 / 86400.0, 1.0 / 100.0 / 86400.0]))
        self.sand_fracs = self.array_to_list_entry(np.array([0.03, 0.03, 0.03]))
        self.clay_fracs = self.array_to_list_entry(np.array([0.5, 0.5, 0.5]))
        self.organic_matter_fracs = self.array_to_list_entry(np.array([0.05, 0.05, 0.05]))

        # Residual soil water content [1]
        self.theta_r =  self.array_to_list_entry(np.array([0.105, 0.105, 0.105]))
        # Saturation soil water content [1]
        self.theta_s =  self.array_to_list_entry(np.array([0.426, 0.426, 0.426]))
        #Campbell 1974/ Clapp and STuff 1978 shape parameter [1]
        self.camp_b =  self.array_to_list_entry(np.array([8.4, 8.4, 8.4]))

        # Reference Soil water potential [m]
        self.psi_soil_sat =  self.array_to_list_entry(np.array([-0.153, -0.153, -0.153]))
        # Reference Soil water potential [m]
        self.pore_size_ind =  self.array_to_list_entry(np.array([0.5, 0.5, 0.5]))


        # Solver precision
        self.solver_precision = 1E-10


        # Number of trees per m-2
        self.tree_density = 1/20.0


        self.sigma_log_likelyhood = 1.0

        #Only relevant for Swiss soils at the moments
        self.soil_profile_index = 2;

    def array_to_list_entry(self, array):
        array_s = np.array2string(array, separator=';')
        return array_s[1:-1]