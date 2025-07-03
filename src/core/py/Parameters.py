from enum import Enum
import numpy as np


class Conductivity_Fraction_Module_Type(Enum):
    Weibull = 0
    Logit = 1

class Soil_Water_Model_Type(Enum):
    VanGenuchten = 0
    Saxton06 = 1
    Campbell = 2

class Stem_Flow_Model_Type(Enum):
    Linear = 0
    KirchhoffWeibull = 1
    KirchhoffPiecewiseErf = 2


class SoilLayer:
    def __init__(self):
        # Organic matter fraction [0-1]
        self.organic_matter_fraction = 0.05
        # Sand fraction [0-1]
        self.sand_fraction = 0.03
        # Clay fraction [0-1]
        self.clay_fraction = 0.4
        # Saturated soil hydraulic conductivity [m s-1]
        self.k_soil_sat = 1.0 / 86400.0
        # Soil layer depth [m]
        self.depth = 0.1
        # Saturated volumetric water content [m3 m-3]
        self.theta_s = 0.6
        # Residual/air entry volumetric water content [m3 m-3]
        self.theta_r = 0.105
        # Reference Soil water potential in hydraulic head [m]
        # from Clapp 1978: -15.3 cm
        self.psi_soil_sat = -0.153
        # Needed for van genuchten [-]
        self.pore_size_ind = 0.5;
        # Campbell 1974 / Clapp and Stuff 1978 shape parameter [1]
        self.camp_b = 8.4;


class Parameters:
    def __init__(self):
        # Universal parameter id
        self.id = -1
        # Root area index [1]
        # Katul et al: 5.5 - 14.2
        self.root_area_index = 12
        # Plant height [m]
        self.canopy_height = 31
        
        
        # Simulation timestep length [s]
        self.dts = 1800.0
        # Input timestep length [s]
        self.dts_input = 1800
        # Extended shell output
        self.verbose = False
        # Maximum leaf water potential change per hour [MPa]
        self.max_psi_leaf_change_per_hour = 1.0;
        # Multiplier to estimate the minimum leaf water potential
        # psi_leaf_min = minimum_psi_leaf_multiplier x psi_88 [-]
        self.minimum_psi_leaf_multiplier = 1.75;
        # Mathematical shape how the loss of conductivity function is calculated
        self.conductivity_fraction_type = Conductivity_Fraction_Module_Type.Weibull.name


        self.vmax25 = 40
        self.jmax25 = 70


        # Root area index [1]
        # Katul et al: 5.5 - 14.2
        self.root_area_index = 12
        # Plant height [m]
        self.canopy_height = 31

        # Stemflow type. [enum]
        # This is essential how we solve the Kirchhoff integral for the water flow
        # J = \int^{\psi_\mathrm{Leaf}}_{{\psi_\mathrm{Stem}}} k_{\mathrm{x}}(\psi) d\psi
        self.stem_flow_type = Stem_Flow_Model_Type.Linear.name

        # Number of segments into which the stem will be divided into
        # to approximate the Kirchhoff integral
        # Should be at least 5
        self.n_stem_segments = 10

        # Viscosity of the leaf to sap flow [1] ??? To be double checked
        self.eta_LS = 1.0  # 1.0 = Water
        # Stem hydraulic capacity [mol m-3 MPa-1]
        # From Meinzer et al. 2011 Figure 13.2
        # Range: 10 - 500 kg m-3 MPa-1
        c_kg = 200
        self.stem_hydraulic_capacitance = c_kg * 1000.0 / 18.0
        # Xylem saturated Hydraulic conductivity [mol m-2 s-1 MPa]
        # from xu et al 2016: 1.7 - 7.5 kg H2O m-2 s-1
        # 1 mole of water equals 18g
        # equals to 93 - 450 mol m-2 s-1 MPa
        self.k_xylem_sat = 50
        # Huber value [m2 m-2] equals 1/k_latosa
        # LPJ-GUESS ranges covers 1:2000 to 1:50000
        self.huber_value = 1.0 / 3600.0
        # Xylem water potential at loss of 50% conductivity [MPa]
        self.psi50_xylem = -3.5
        # Xylem water potential at loss of 50% conductivity [MPa]
        self.psi88_xylem = -6.0
        # Experimental and should only be used with longer time series
        self.sustain_xylem_damage = False
        # Parameter describing at which levels xylem damage is permanent
        self.permanent_xylem_fraction_threshold = 0.12

        # Leaf hydraulic capacitance [mol m-2 MPa]
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

        # Medlyn 2011 model g0 parameter [mol m-2 s-1 leaf area]
        # (also minmal stomatal condutances)
        # From Medlyn 0 - 0.05
        # From the site 0.05
        self.g0 = 0.001
        # Medlyn 2011 model g1 parameter [kPa -1]
        # From Medlynn 1.6 - 12
        self.g1 = 1.5

        # Minimum bark conductance
        # Todo add unit
        self.g_stem_res = 0.0
        
        # Conductivity/Diffusivity of the heart to sapwood 
        self.k_heart_sap = 0.0
        
        # Ratio of heartwood to sapwood area
        self.ratio_heart_sap_area = 3.0

        # Soil depths
        self.soil_depths = np.repeat(0.1, 3);
        self.soil_depths = np.array2string(self.soil_depths, separator=';')
        self.soil_depths = self.soil_depths[1:-1]

        # Jackson rooting parameter [-]
        # This parameter is describing how roots are distributed
        # across soil depth
        # Note: In the model this used as a proportionality factor for water uptake
        # but in reality water can come from deeper layers
        self.jackson_root_beta = 0.96

        # Soil water retention curve model
        # Could be either VanGenuchten, Campbell, or Saxton06.
        # Saxton06 is experimental at the moment
        # Note: Depending on the selected model not all parameters of the soil layers are used
        self.soil_water_model_type = Soil_Water_Model_Type.VanGenuchten.name

        # DO NOT CHANGE THESE PARAMETERS
        # Saturated soil hydraulic conductance [m s-1]
        self.k_soil_sats = self.array_to_list_entry(
            np.array([1.0 / 100.0 / 86400.0, 1.0 / 100.0 / 86400.0, 1.0 / 100.0 / 86400.0]))
        # Reference Soil water potential [m]
        self.psi_soil_sats = self.array_to_list_entry(np.array([-0.153, -0.153, -0.153]))
        # Sand fractions between 0 and 1 [1]
        self.sand_fracs = self.array_to_list_entry(np.array([0.03, 0.03, 0.03]))
        # Clay fractions between 0 and 1 [1]
        self.clay_fracs = self.array_to_list_entry(np.array([0.5, 0.5, 0.5]))
        # Organic matter fractions between 0 and 1 [1]
        self.organic_matter_fracs = self.array_to_list_entry(np.array([0.05, 0.05, 0.05]))
        # Residual soil water content [1]
        self.theta_r = self.array_to_list_entry(np.array([0.105, 0.105, 0.105]))
        # Saturation soil water content [1]
        self.theta_s = self.array_to_list_entry(np.array([0.426, 0.426, 0.426]))
        # Campbell 1974/ Clapp and STuff 1978 shape parameter [1]
        self.camp_b = self.array_to_list_entry(np.array([8.4, 8.4, 8.4]))
        # Van genuchten parameter [?]
        self.pore_size_ind = self.array_to_list_entry(np.array([0.5, 0.5, 0.5]))

        # Number of trees per m-2
        # Important parameter to scale up/down the sapflux
        self.tree_density = 1.0 / 50.0

        # Parameter needed baysian parameter estimation
        # Todo adapt if multiple LL functions are being calculated
        self.sigma_log_likelyhood = 1.0

        # Only relevant for Swiss soils at the moment
        self.soil_profile_index = 2;

        # Multiplier for the soil water content of each layer
        # Should only be used for testing purposes
        self.wcont_sigma_deviation = 0.0

        # Solver precision
        self.solver_precision = 1E-10

    @staticmethod
    def array_to_list_entry(array):
        array = np.array(array)
        array_s = np.array2string(array, separator=';')
        return array_s[1:-1]

    def Create_soil_layers(self):
        depths = self.soil_depths.split(';')
        organic_matter_fractions = self.organic_matter_fracs.split(';')
        sand_fractions = self.sand_fracs.split(';')
        clay_fractions = self.clay_fracs.split(';')
        k_soil_sats = self.k_soil_sats.split(';')
        psi_soil_sats = self.psi_soil_sats.split(';')

        theta_rs = self.theta_r.split(';')
        theta_ss = self.theta_s.split(';')
        pore_size_inds = self.pore_size_ind.split(';')
        camp_bs = self.camp_b.split(';')

        soil_layers = []
        for i in range(len(depths)):
            layer = SoilLayer()
            layer.depth = float(depths[i])
            layer.organic_matter_fraction = float(organic_matter_fractions[i])
            layer.sand_fraction = float(sand_fractions[i])
            layer.clay_fraction = float(clay_fractions[i])
            layer.k_soil_sat = float(k_soil_sats[i])
            layer.psi_soil_sat = float(psi_soil_sats[i])
            layer.theta_r = float(theta_rs[i])
            layer.theta_s = float(theta_ss[i])
            layer.pore_size_ind = float(pore_size_inds[i])
            layer.camp_b = float(camp_bs[i])
            soil_layers.append(layer)
        return soil_layers

    def Create_CParameters(self, soil_layers):

        from SimPHony import Soil_water_module_type as CSoil_water_module_type
        from SimPHony import Stem_flow_module_type as CStem_flow_module_type
        from SimPHony import CParameters
        from SimPHony import CSoil_layer

        cparameters = CParameters()

        cparameters.id = self.id

        if self.soil_water_model_type == Soil_Water_Model_Type.Campbell.name:
            cparameters.soil_water_type = CSoil_water_module_type.Campbell
        elif self.soil_water_model_type == Soil_Water_Model_Type.VanGenuchten.name:
            cparameters.soil_water_type = CSoil_water_module_type.VanGenuchten
        elif self.soil_water_model_type == Soil_Water_Model_Type.Saxton06.name:
            cparameters.soil_water_type = CSoil_water_module_type.Saxton06
        else:
            print("Invalid soil water module type")

        if self.stem_flow_type == Stem_Flow_Model_Type.Linear.name:
            cparameters.stem_flow_type = CStem_flow_module_type.Linear
        elif self.stem_flow_type == Stem_Flow_Model_Type.KirchhoffWeibull.name:
            cparameters.stem_flow_type = CStem_flow_module_type.KirchhoffWeibull
        elif self.stem_flow_type == Stem_Flow_Model_Type.KirchhoffPiecewiseErf.name:
            cparameters.stem_flow_type = CStem_flow_module_type.KirchhoffPiecewiseErf
        else:
            print("Invalid stem flow type")

        clayers = []
        for layer in soil_layers:
            clayer = CSoil_layer()
            clayer.depth = layer.depth

            clayer.organic_matter_fraction = layer.organic_matter_fraction
            clayer.sand_fraction = layer.sand_fraction
            clayer.clay_fraction = layer.clay_fraction

            clayer.k_soil_sat = layer.k_soil_sat
            clayer.psi_soil_sat = layer.psi_soil_sat
            clayer.theta_r = layer.theta_r
            clayer.theta_s = layer.theta_s
            clayer.pore_size_ind = layer.pore_size_ind
            clayer.camp_b = layer.camp_b
            clayers.append(clayer)
        cparameters.soil_layers = np.array(clayers)

        cparameters.root_area_index = self.root_area_index
        cparameters.canopy_height = self.canopy_height
        cparameters.stem_hydraulic_capacitance = self.stem_hydraulic_capacitance
        cparameters.k_xylem_sat = self.k_xylem_sat
        cparameters.huber_value = self.huber_value
        cparameters.psi50_xylem = self.psi50_xylem
        cparameters.psi88_xylem = self.psi88_xylem
        cparameters.leaf_hydraulic_capacitance = self.leaf_hydraulic_capacitance
        cparameters.leaf_area_index = self.leaf_area_index
        cparameters.psi_leaf_50_close = self.psi_leaf_50_close
        cparameters.d_50_close = self.d_50_close
        cparameters.g0 = self.g0
        cparameters.g1 = self.g1
        cparameters.g_stem_res = self.g_stem_res
        cparameters.jackson_root_beta = self.jackson_root_beta
        cparameters.wcont_sigma_deviation = self.wcont_sigma_deviation
        cparameters.dts_input = self.dts_input
        cparameters.tree_density = self.tree_density
        cparameters.n_stem_segments = self.n_stem_segments
        cparameters.sustain_xylem_damage = self.sustain_xylem_damage
        cparameters.permanent_xylem_fraction_threshold = self.permanent_xylem_fraction_threshold
        cparameters.sigma_log_likelyhood = self.sigma_log_likelyhood
        cparameters.max_psi_leaf_change_per_hour = self.max_psi_leaf_change_per_hour
        cparameters.minimum_psi_leaf_multiplier = self.minimum_psi_leaf_multiplier
        cparameters.solver_precision = self.solver_precision
        cparameters.verbose = self.verbose
        cparameters.dts = self.dts
        cparameters.soil_profile_index = self.soil_profile_index
        cparameters.vmax25 = self.vmax25
        cparameters.jmax25 = self.jmax25
        cparameters.k_heart_sap = self.k_heart_sap
        cparameters.ratio_heart_sap_area = self.ratio_heart_sap_area

        return cparameters


def Convert_Soil_Parameters(soil_layers: [SoilLayer], parameters: Parameters):
    k_soil_sats = []
    psi_soil_sats = []
    sand_fracs = []
    clay_fracs = []
    organic_matter_fracs = []
    theta_r = []
    theta_s = []
    camp_b = []
    pore_size_ind = []
    depth = []

    for layer in soil_layers:
        k_soil_sats.append(layer.k_soil_sat)
        psi_soil_sats.append(layer.psi_soil_sat)
        sand_fracs.append(layer.sand_fraction)
        clay_fracs.append(layer.clay_fraction)
        organic_matter_fracs.append(layer.organic_matter_fraction)
        theta_r.append(layer.theta_r)
        theta_s.append(layer.theta_s)
        camp_b.append(layer.camp_b)
        pore_size_ind.append(layer.pore_size_ind)
        depth.append(layer.depth)

    parameters.k_soil_sats = parameters.array_to_list_entry(k_soil_sats)
    parameters.psi_soil_sats = parameters.array_to_list_entry(psi_soil_sats)
    parameters.sand_fracs = parameters.array_to_list_entry(sand_fracs)
    parameters.clay_fracs = parameters.array_to_list_entry(clay_fracs)
    parameters.organic_matter_fracs = parameters.array_to_list_entry(organic_matter_fracs)
    parameters.theta_r = parameters.array_to_list_entry(theta_r)
    parameters.theta_s = parameters.array_to_list_entry(theta_s)
    parameters.camp_b = parameters.array_to_list_entry(camp_b)
    parameters.pore_size_ind = parameters.array_to_list_entry(pore_size_ind)
    parameters.soil_depths = parameters.array_to_list_entry(depth)
