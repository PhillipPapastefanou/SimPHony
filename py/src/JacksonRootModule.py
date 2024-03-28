import numpy as np
from src.py.Parameters import Parameters
class Jackson_Root_Distribution:
    def __init__(self, params:  Parameters):
        # Depth of each soil layer [m]
        self.soil_depths = params.soil_depths

        # Jackson rooting parameter [-]
        self.jackson_root_beta  = params.jackson_root_beta

    def Calculate_Root_Distribution(self):

        root_fracs =  np.zeros(self.soil_depths.shape[0])

        indices = np.arange(self.soil_depths[0], np.sum(self.soil_depths) + self.soil_depths[0], self.soil_depths[0])

        r_i = 0
        for i in indices:
            root_fracs[r_i] = np.power(self.jackson_root_beta, 100.0*i) - np.power(self.jackson_root_beta, 100.0*(i + self.soil_depths[0]))
            r_i += 1

        # Ensure we have distributed the roots across all available layers
        root_fracs/= np.sum(root_fracs)



        if indices.shape != root_fracs.shape:
            raise Exception("Invalid root fractions")

        return root_fracs
