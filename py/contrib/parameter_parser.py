from src.Parameters import Parameters
from src.Parameters import Conductivity_Fraction_Module_Type
from src.Parameters import Stem_Flow_Model_Type
from src.Parameters import Soil_Water_Model_Type
from src.Parameters import SoilLayer
import pandas as pd

class Parameter_Parser():

    def __init__(self):
        self.parameters_list = []
        self.soil_layers_list = []

    def Add(self, parameters:  Parameters):
        self.parameters_list.append(parameters)

    def Write_First_File(self, filename):

        # only need to this for the first element only
        #p1 = self.parameters_list[0]
        #self.vars = [a for a in dir(p1) if not a.startswith('__') and not callable(getattr(p1, a))]

        dicts = []
        for params in self.parameters_list:
             dicts.append(vars(params))

        self.df = pd.DataFrame(dicts)
        self.df.iloc[0:1].to_csv(filename, index=True)

    def Write_Full_Parameter_File(self, filename):

        # only need to this for the first element only
        #p1 = self.parameters_list[0]
        #self.vars = [a for a in dir(p1) if not a.startswith('__') and not callable(getattr(p1, a))]

        dicts = []
        for params in self.parameters_list:
             dicts.append(vars(params))
        self.df = pd.DataFrame(dicts)
        self.df.to_csv(filename, index=True)

    def Write_Partial_Parameter_File(self, filename, selected_params):

            # only need to this for the first element only
            # p1 = self.parameters_list[0]
            # self.vars = [a for a in dir(p1) if not a.startswith('__') and not callable(getattr(p1, a))]

            dicts = []
            dictfilt = lambda x, y: dict([(i, x[i]) for i in x if i in set(y)])
            for params in self.parameters_list:
                all_vars = vars(params)
                filt_vars  = dictfilt(all_vars, selected_params)
                dicts.append(filt_vars)
            self.df = pd.DataFrame(dicts)
            self.df.to_csv(filename, index=True,  float_format='%.4g')


    def Read_Parameter_List(self, filename):

        self.parameters_list.clear()
        self.soil_layers_list.clear()

        df = pd.read_csv(filename)

        params = Parameters()
        var_list = vars(params)

        for name in var_list:
            if hasattr(params, name):
                if name in df.columns:
                    setattr(params, name,  df.loc[0, name])
                else:
                    print(f"could not find {name}")

        params.conductivity_fraction_type = Conductivity_Fraction_Module_Type[params.conductivity_fraction_type].name
        params.stem_flow_type = Stem_Flow_Model_Type[params.stem_flow_type].name
        params.soil_water_model_type = Soil_Water_Model_Type[params.soil_water_model_type].name

        soil_layers = params.Create_soil_layers()

        self.soil_layers_list.append(soil_layers)
        self.parameters_list.append(params)







        self.parameters_list.append(params)


