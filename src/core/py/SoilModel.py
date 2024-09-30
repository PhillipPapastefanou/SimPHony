from enum import Enum
import numpy as np

class VanGenuchten_Water_Uptake:
    def __init__(self, params):

        self.theta_s = params.theta_s
        self.theta_r = params.theta_r
        self.alpha = params.alpha_genucht
        self.k_soil_sat = params.k_soil_sat
        self.n = params.n_genucht
        self.m = 1.0 - 1.0 / self.n
        self.neta = params.neta_genucht

    def Calculate_PsiSoil_KSoil(self, theta):

        self.thetas = theta
        # Relative fraction [-]
        self.se = (-theta + self.theta_r)/(self.theta_r - self.theta_s)
        self.psi_soil_array = -1.0/100.0*(-self.se**(-1.0/self.m) * (-1.0 + self.se**(1.0 / self.m)) * self.alpha**(-1.0 *self.n))
        self.k_soil_array = self.k_soil_sat *self.se ** self.neta * (1.0 - (1.0 - self.se**(1.0/self.m))**self.m)**2.0
        #self.k_soil_array = self.k_soil_sat * (theta/self.th_s)**(2.0 + 3.0* self.b)
        #self.psi_soil_array = self.psi_ref * (theta/self.th_s)**(-self.b)

class Campbell_Water_Uptake:
    def __init__(self, params):

        self.theta_s = params.theta_s
        self.k_soil_sat = params.k_soil_sat
        self.b = params.camp_b
        self.ps_soil_ref = params.camp_psi_soil_ref

    def Calculate_PsiSoil_KSoil(self, theta):
        self.thetas = theta
        self.k_soil_array = self.k_soil_sat * (self.thetas/self.theta_s) ** (2.0  + 3.0*self.b)
        self.psi_soil_array = self.ps_soil_ref * (self.thetas/self.theta_s) ** (-self.b)