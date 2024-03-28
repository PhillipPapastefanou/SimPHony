import numpy as np
from src.py.JacksonRootModule import Jackson_Root_Distribution
from src.py.SoilModel import VanGenuchten_Water_Uptake
from src.py.SoilModel import Campbell_Water_Uptake

class ML_Leaf_Stem_SemiCoupled_Module:
    def __init__(self, params):
        # Hydraulic Xylem conductivity [mol m-2 s-1 MPa]
        self.C_L = params.leaf_hydraulic_capacitance
        # Leaf area index [m2 m-2]
        self.LAI = params.leaf_area_index
        # Leaf water potential at 50% downregulation of stomata [MPa]
        self.psi_leaf_50_close = params.psi_leaf_50_close

        # Shape of downregulation speed
        self.d_50_close = params.d_50_close

        self.beta_c50 = self.psi_leaf_50_close + np.log(np.log(2.0)) / self.d_50_close

        # Medlyn 2011 model g0 parameter [mol m-2 s-1]
        # (also minmal stomatal condutances)
        self.g0 = params.g0
        # Medlyn 2011 model g1 parameter [1]
        self.g1 = params.g1

        # Hydraulic Xylem conductivity [mol m-2 s-1 MPa]
        self.kLS = params.k_xylem_sat
        # Dynamic? viscosity of stem-leaf-continuum [1]
        self.eta_LS = params.eta_LS
        # Plant height [m]
        self.h = params.canopy_height
        # Density of water [kg m-3]
        self.rho = params.rho_water
        # Gravitational constant
        self.g = params.grav
        # Huber value [m m-1] equals 1/klatosa
        self.huber = params.huber_value

        # Campbell 1974/ Clapp and STuff 1978 shape parameter [1]
        self.b = params.camp_b
        # Saturation soil water content [1]
        self.th_s  = params.theta_s
        # Reference Soil water potential [MPa]
        self.psi_ref = params.camp_psi_soil_ref
        # Hydraulic conductivity [m s-1]
        self.k_soil_sat = params.k_soil_sat

        # Root zone depth [m]
        self.Li = params.root_zone_depth
        # Root area index [1]
        self.RAI = params.root_area_index
        # Stem hydraulic capacity [mol m-3 MPa-1]
        self.CS = params.stem_hydraulic_capacitance

        # Root distributions
        j_dist = Jackson_Root_Distribution(params)
        self.root_fractions = j_dist.Calculate_Root_Distribution()


        #self.soil_water_uptake = VanGenuchten_Water_Uptake(params)
        self.soil_water_uptake = Campbell_Water_Uptake(params)



        self.PaToMPa = 10 ** (-6)
        self.MPaToPa = 10 ** (6)
        self.eps = 1E-6

        self.nsoil = self.root_fractions.shape[0]

        self.times = []
        self.psi_leaves = []
        self.psi_stems = []
        self.psi_soils = []
        self.psi_soil_avg = []

        self.Js = []
        self.Ts = []
        self.Gs = []
        self.G_indiv = np.zeros(self.nsoil)
        self.GS_i_out = []

        self.gss = []

        self.betas = []

        self.out_vpds = []
        self.out_vwc_array = []

        self.out_vwc_upper = []
        self.out_vwc_lower = []

        self.steps_S = []
        self.steps_L = []

    def set_initial_conditions(self, psi_leaf_zero, psi_stem_zero):
        self.psi_leaf = psi_leaf_zero
        self.psi_stem = psi_stem_zero

        #self.psi_leaves.append(psi_leaf_zero)
        #self.psi_stems.append(psi_stem_zero)
        #self.times.append(sr)


    def getTimeIndex(self, seconds_elapsed):
        return int(seconds_elapsed / 1800.0)

    def set_drivers(self, anet, c_a, pressure, vpd, theta):
        self.anet_array = anet
        self.c_a = c_a
        self.pressure = pressure
        self.vpd_array = vpd
        self.theta_array = theta
        self.soil_water_uptake.Calculate_PsiSoil_KSoil(theta)

        self.k_soil_array = self.soil_water_uptake.k_soil_array
        self.psi_soil_array = self.soil_water_uptake.psi_soil_array

        x = 3


    def update_stem_water_flow_J(self, psi_leaf, psi_stem):

        # Update Forcing pressure between leaves and stem [MPa]
        self.DeltaP_LS = psi_stem - psi_leaf - (self.rho * self.g * self.h / 2.0) * self.PaToMPa

        # Prevent negative pressure differences to avoid letting the water flow up the tree
        if self.DeltaP_LS < 0.0:
            self.DeltaP_LS = 0.0

        # Calculate stem water flow [mol m-2 s-1]
        return self.DeltaP_LS * self.kLS * self.huber / (self.eta_LS * self.h / 2.0)

    def dpsiL(self, psi_leaf, psi_stem):

        self.beta =  np.exp( - np.exp(-self.d_50_close * (psi_leaf - self.beta_c50)))

        #self.beta = 1.0 / (1.0 + np.exp(-self.d_50_close * (psi_leaf - self.psi_leaf_50_close)))

        self.gs = self.g0 + self.beta  * (1.0 + self.g1 / np.sqrt(self.vpd / self.pressure)) * self.anet / self.c_a

        # Convert from Mol CO2 to Mol H2O
        self.gs *= 1.6

        self.J = self.update_stem_water_flow_J(psi_leaf, psi_stem)

        self.T = self.gs * self.LAI * self.vpd / self.pressure

        return ((self.J - self.T)/ self.C_L)


    def dpsiS(self, psi_leaf, psi_stem):

        self.G = 0.0
        for i in range(0, self.nsoil):
            self.G_indiv[i] = self.root_fractions[i] * self.k_soil[i] * np.sqrt(self.RAI) / np.pi / self.Li * (self.psi_soil[i] - psi_stem - (
                    self.rho * self.g * self.h / 2.0) * self.PaToMPa) / self.g * self.MPaToPa

            # Avoid water from flowing down from the stem to the soil
            if self.G_indiv[i] < 0:
                 self.G_indiv[i] = 0.0

            self.G += self.G_indiv[i]


        return ((self.G - self.J) / (self.CS * self.h * self.huber))

    def _add_output(self):
        self.times.append(self.t_r)
        self.psi_leaves.append(self.psi_leaf)
        self.psi_stems.append(self.psi_stem)
        self.psi_soils.append(self.psi_soil)
        self.Js.append(self.J)
        self.Ts.append(self.T)
        self.Gs.append(self.G)

        psi_soil_avg = 0.0
        for i in range(0, self.nsoil):
            psi_soil_avg += self.root_fractions[i] * self.psi_soil[i]


        self.psi_soil_avg.append(psi_soil_avg)
        self.GS_i_out.append(self.G_indiv)
        #self.psi_soils_i_out.append(self.psi_soils_i)

        self.gss.append(self.gs)
        self.betas.append(self.beta)
        self.out_vpds.append(self.vpd)
        self.out_vwc_upper.append(self.theta_array[self.getTimeIndex(self.t_r), 0])
        self.out_vwc_lower.append(self.theta_array[self.getTimeIndex(self.t_r), self.nsoil-1])



    def Update_Euler_Explicit(self, steplen, timestart_s, timeend_s):


        deltaT = timeend_s - timestart_s
        nsteps = deltaT/steplen


        self.t_r = timestart_s
        for i in range(0, int(nsteps)):


            self.anet = self.anet_array[self.getTimeIndex(self.t_r)]
            self.c_a  = self.c_a
            self.pressure =   self.pressure
            self.vpd = self.vpd_array[self.getTimeIndex(self.t_r)]
            self.k_soil = self.k_soil_array[self.getTimeIndex(self.t_r)]
            self.psi_soil = self.psi_soil_array[self.getTimeIndex(self.t_r)]

            dL = self.dpsiL(self.psi_leaf, self.psi_stem)
            dS = self.dpsiS(self.psi_leaf, self.psi_stem)

            self.psi_leaf += dL * steplen
            self.psi_stem += dS * steplen

            self._add_output()



            self.t_r += steplen
            #print(f"{self.psi_leaf}  {self.psi_stem}")
            #print(f"{dL}  {dS}")
            #print(f"{self.G} {self.J} {self.T}")


    def _psiS_root(self, psi_stem_r, psi_stem_x, psi_leaf,  steplen):
        d_psi_stem_r = self.dpsiS(psi_leaf, psi_stem_x)
        return psi_stem_r + d_psi_stem_r * steplen - psi_stem_x

    def _psiL_root(self, psi_leaf_r, psi_leaf_x, psi_stem,  steplen):
        d_psi_leaf_r = self.dpsiL(psi_leaf_x, psi_stem)
        return psi_leaf_r + d_psi_leaf_r * steplen - psi_leaf_x


    def Update_Euler_Implicit(self, steplen, timestart_s, timeend_s):

        # Delta t in seconds
        deltaT = (timeend_s - timestart_s)
        nsteps = deltaT/steplen

        self.eps = 1E-10

        nerrors_max = 10
        nerrors = 0

        self.t_r = timestart_s
        for i in range(0, int(nsteps)):
            self.anet = self.anet_array[self.getTimeIndex(self.t_r)]
            self.c_a = self.c_a
            self.pressure = self.pressure
            self.vpd = self.vpd_array[self.getTimeIndex(self.t_r)]
            self.k_soil = self.k_soil_array[self.getTimeIndex(self.t_r)]
            self.psi_soil = self.psi_soil_array[self.getTimeIndex(self.t_r)]


            s0 = -15
            s1 = 0.0
            step = 1
            condition = True
            while condition:
                s2 = (s0 + s1) / 2

                y0 = self._psiL_root(self.psi_leaf, s0, self.psi_stem,  steplen)
                y2 = self._psiL_root(self.psi_leaf, s2, self.psi_stem,  steplen)

                if y0 * y2 < 0:
                        s1 = s2
                else:
                        s0 = s2
                step = step + 1
                condition = abs(y2) > self.eps

                if step > 100:
                    if nerrors < nerrors_max:
                        print(
                            f"Leaf water solver did not converge with inital conditions ({-15},{0}) at"
                            f" timestep {self.t_r}s. Using most negative psi_leaf of timestep before.")
                        print(f'Previous psi_leaf {self.psi_leaf}')
                        s2 = self.psi_leaf
                        nerrors += 1
                    break

            self.psi_leaf = s2
            self.steps_L.append(step)



            s0 = self.psi_leaf * 10
            s1 = 0
            step = 1
            condition = True
            while condition:
                s2 = (s0 + s1) / 2

                y0 = self._psiS_root(self.psi_stem, s0, self.psi_leaf,  steplen)
                y2 = self._psiS_root(self.psi_stem, s2, self.psi_leaf,  steplen)

                if y0 * y2 < 0:
                        s1 = s2
                else:
                        s0 = s2
                step = step + 1
                condition = abs(y2) > self.eps


                if step > 100:
                    if nerrors < nerrors_max:
                        print(f"Stem water solver did not converge with inital conditions ({np.round(self.psi_leaf * 10, 3)},{0}) at"
                              f" timestep {self.t_r}s. Using most negative psi_leaf of timestep before.")
                        s2 = self.psi_stem
                        nerrors += 1
                    break



            self.psi_stem = s2


            self.steps_S.append(step)


            self._add_output()

            # if self.psi_stem < - 7.0:
            #     break;

            self.t_r += steplen
        print(f"psi_leaf {self.psi_leaf}")
        print(f"psi_stem {self.psi_stem}")