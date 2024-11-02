from enum import Enum

class Location(Enum):
    Invalid = -1
    Swiss_cc = 0
    Hainich = 1

class Swiss_soil_water_input_type(Enum):
    NLayers_Mean = 0
    NLayers_Mean_One_Std = 1
    NLayers_Mean_N_Std = 2
    NLayers_Indiv = 3



class Config:
    def __init__(self):
        self.location = Location.Invalid
        self.config_file = ""
        self.forcing_file  = ""
        self.parameters_list_file  = ""
        self.parameter_input_file_list_partial  = ""
        self.soilwater_file  = ""
        self.build_path = ""
        self.lib_path  = ""
        self.psi_stem_file  = ""
        self.swiss_tree_folder_path  = ""
        self.swiss_soil_water_input_type = Swiss_soil_water_input_type.NLayers_Mean
        self.sap_file = ""
        self.output_path = ""
        self.post_path = ""
        self.input_path = ""
        self.scenario_path = ""
        self.nsims = -1
        self.nbest = 30

    def Export(self, filename):
        variables = vars(self)
        lines = []
        for var in variables:

            value = variables[var]
            if isinstance(value, Enum):
                str_value = value.name
            else:
                str_value = value

            str_row = f'{var}={str_value}'
            lines.append(str_row)

        with open(filename, 'w') as f:
            for line in lines:
                f.write(f"{line}\n")

