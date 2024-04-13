from src.Parameters import Parameters
import pandas as pd

class ParametersList():

    def __init__(self):
        self.parameters_list = []


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