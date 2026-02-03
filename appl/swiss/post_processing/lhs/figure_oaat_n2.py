import unittest
import sys
import os
import copy
import numpy as np
import pandas as pd
import datetime
import matplotlib.pyplot as plt
print(sys.version)
print(sys.executable)


label_dict  = {}
label_dict['huber_value'] = r"Huber $\mathcal{H}$"
label_dict['stem_hydraulic_capacitance'] = r"$\Gamma_\mathrm{Stem}$"
label_dict['stem_hydraulic_capacitance_res'] = r"$\Gamma_\mathrm{Stem_res}$"
label_dict['leaf_hydraulic_capacitance'] = r"$\Gamma_\mathrm{Leaf}$"
label_dict['g0'] = r"$g_0$"
label_dict['g1'] = r"$g_1$"
label_dict['k_xylem_sat'] = r"$K_\mathrm{xyl, max}$"
label_dict['psi50_xylem'] = r"$\psi_{50}$"
label_dict['psi88_xylem'] = r"$\psi_{88}$"
label_dict['wcont_sigma_deviation'] = r"$\sigma_\mathrm{wcont}$"
label_dict['pore_0'] = r"$\xi$"
label_dict['jackson_root_beta'] = r"$\mathrm{Root}_\beta$"
label_dict['psi_leaf_50_close'] = r"$\psi_{Leaf, close}$"
label_dict['d_50_close'] = r"$d_{Leaf, close}$"
label_dict['alpha'] = r"$\alpha$"
label_dict['gam_stem_x_H'] = r"$\Gamma_\mathrm{Stem} \cdot \mathcal{H}$"
label_dict['gam_stem_div_g0'] = r"$\frac{g_0}{\Gamma_\mathrm{Stem}}$"
label_dict['k_xyl_x_H'] = r"$K_\mathrm{xyl, max}\cdot \mathcal{H}$"
label_dict['g_min_loss'] = r"$g_0 + g_\mathrm{Bark}$"
label_dict['g_min_loss'] = r"$g_0 + g_\mathrm{Bark}$"
label_dict['wcont_sigma_deviation'] = r"$\sigma_\mathrm{wcont}$"
label_dict['p50-p88'] =  r"$\psi_{50} - \psi_{88} $"
label_dict['g0_div_gam_stem_x_H'] =  r"g0_div_gam_stem_x_H"
label_dict['g0_div_gam_stem_x_H'] =  r"g0_div_gam_stem_x_H"
label_dict['k_soil_sat0'] =  r"k_soil_sat"
label_dict['root_area_index'] =  r"RAI"
label_dict['psi_tlp'] = r'$\psi_\mathrm{tlp}$'
label_dict['g_stem_res'] = r'$g_\mathrm{stem,res}$'



unit_dict= {}
unit_dict['stem_hydraulic_capacitance'] = "kg m-3 MPa-1"
unit_dict['stem_hydraulic_capacitance_res'] = "kg m-3 MPa-1"
unit_dict['leaf_hydraulic_capacitance'] = "kg m-2 MPa-1"
unit_dict['k_xylem_sat'] = "kg m-1 s-1 MPa-1"
unit_dict['huber_value'] = "-"
unit_dict['psi50_xylem'] = "MPa"
unit_dict['psi88_xylem'] = "MPa"
unit_dict['g0'] = "mol m-1 s-1"
unit_dict['g1'] = "-"
unit_dict['g_stem_res'] = "mol m-1 s-1"
unit_dict['psi_leaf_50_close'] = r"MPa"
unit_dict['d_50_close'] = "-"
unit_dict['alpha'] = r"-"
unit_dict['gam_stem_x_H'] = r"-"
unit_dict['gam_stem_div_g0'] = r"-"
unit_dict['k_xyl_x_H'] =r"-"
unit_dict['g_min_loss'] = r"-"
unit_dict['wcont_sigma_deviation'] = r"-"
unit_dict['p50-p88'] =  "MPa"
unit_dict['psi_tlp'] =  "MPa"
unit_dict['g0_div_gam_stem_x_H'] =  "-"
unit_dict['pore_0'] =  "-"
unit_dict['k_soil_sat0'] =  "m s-1"
unit_dict['root_area_index'] =  "m2 m-2"
unit_dict['g_stem_res'] = r's'


sys.path.append("/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone")
from src.contrib.swiss_tree_parser import SwissTreeParser
from src.contrib.auxil.setups import Setup
from src.contrib.auxil.output_df import create_output_df
from src.contrib.auxil.files import get_trees_psi_leaf_folder_path_cc
from src.contrib.config import Config, Swiss_soil_water_input_type

root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss/excon_other_soil_other_cstem"
scenario_name = "big_overview_20_10000p"
root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss/excon_2024_final"
scenario_name = "big_overview_20_10000p_fine_g1fix_relay"

root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss/excon_2025_final"
scenario_name = "big_overview_20_10000p_fine_g1fix_h_kxylem"

root_output_directory = "/Net/Groups/BSI/scratch/ppapastefanou/simulations/SimPHony/swiss_cc/"
scenario_name = "19_ex_con_2025_full_logit_mean_one"

scenario_name = "23_ex_con_2025_full_logit_mean_one_wcont"
scenario_name = "29_ex_con_2025_full_logit_mean_one_wcont_TLP"

rt_output_save_dir = ""
scenario_name = "45_LHS_TLP"
#scenario_name = "46_LHS_no_TLP"
nmax = 1000
N_BEST = 50
# root_output_directory = "/Users/pp/data/Simulations/A08_SimPHony/swiss/excon"
# scenario_name = "old_params_extensive"

input_path = os.path.join(root_output_directory, scenario_name, 'input')
output_path = os.path.join(root_output_directory, scenario_name, 'output')
post_path = os.path.join(root_output_directory, scenario_name, 'post')

setup = Setup()
setup.Apply_default_swiss(Swiss_soil_water_input_type.OneLayer_Mean_One_Std)
setup.Apply_default_paths(root_output_directory, scenario_name)
setup.config.parameters_list_file = os.path.join(setup.config.post_path, "parameters_best_mean_alive.csv")
setup.config.config_file = os.path.join("config_dummy.txt")
# Specifying forcing and evalution data paths
#setup.Export()

pdir = os.path.join(post_path, "fig3")
os.makedirs(pdir, exist_ok=True)
# Read in the Swiss trees
tree_parser = SwissTreeParser(get_trees_psi_leaf_folder_path_cc())

print(setup.config.build_folder)

sys.path.append("/Net/Groups/BSI/work_scratch/ppapastefanou/src/PlantHydraulicStandalone/build")
from SimPHony import Simulation_Single_Swiss
from SimPHony import DateTime



for ai in range(1,6):
    selective_range = [-1, 0,3, 6, 7]

    df_d= []
    rf1 = 0.15
    rf2 = 0.25
    h = np.array([1-rf2,1-rf1, 1, 1+rf1, 1+rf2])
    bid = 200
    nrows = 5

    cols_pos =['tab:red', 'black', 'tab:blue']
    cols_neg =['tab:blue', 'black', 'tab:red']
    labels = ['lower', 'ref', 'higher']

    var_additionals = ['wcont_sigma_deviation',
    'psi_leaf_50_close',
    'p50-p88',
    'k_xylem_sat',
    'stem_hydraulic_capacitance',
    'huber_value',
    'stem_hydraulic_capacitance_res']


    fig = plt.figure(figsize=(10,8))

    i = 1
    for vara in var_additionals:
        
        df= pd.read_csv(os.path.join(post_path, f"All_RMSE_{nmax}_{ai}.csv"))
        first_row = df.iloc[[bid]] 
        df_rep = pd.concat([first_row] * nrows, ignore_index=True)
        
        if vara == "p50-p88":
            margin = first_row["psi50_xylem"].iloc[0]-first_row["psi88_xylem"].iloc[0]
            df_rep['psi88_xylem'] =  first_row["psi50_xylem"].iloc[0] - margin * h 
            cols = cols_pos
            labels_plt = labels
        else:
            df_rep[vara] = first_row[vara].iloc[0] * h
            if first_row[vara].iloc[0] > 0.0:
                cols = cols_pos
                labels_plt = labels
            else:
                cols = cols_neg
                labels_plt =  list(reversed(labels))
                
        print(labels_plt)
        df_rep.to_csv(os.path.join(post_path, f"All_RMSE_{nmax}_{ai}_pclose.csv"))
        
        

        setup.config.parameters_list_file = os.path.join(post_path, f"All_RMSE_{nmax}_{ai}_pclose.csv")
        setup.Export()

        for j in range(0, nrows):
            id = selective_range[0]
            # Specify Start and End of the Simulation
            format = "%Y-%m-%d %H:%M:%S"
            date_start_str = "2018-05-01 00:00:00"
            date_end_str = "2018-12-15 00:00:00"
            timestart = DateTime(date_start_str, format)
            timeend = DateTime(date_end_str, format)

            # Set up the PHS simulation
            # read in the parameter file that we just created
            sim = Simulation_Single_Swiss()
            sim.Read_config(setup.config.config_file)
            sim.Init_eval(timestart, timeend)
            sim.Init_parameters_fn_single(j)
            sim.Init_input()
            sim.Set_water_pot_initials(-1.0, -0.2)

            # Run the simulation
            sim.Run(timestart, timeend)
            sim.Analyse()

            # Get output and analysis data
            # output = sim.Get_output()
            an = sim.Get_analysis()
            errors = an.Get_rmse()

            # Getting the raw output data
            output = sim.Get_output()
            # Getting the raw analysis data
            an = sim.Get_analysis()
                    

            # Creating pandas dataframe
            df_mod = create_output_df(output, date_start_str)

            df_mod['id'] = int(j)

            rmse = an.Get_rmse()
            if ai == 1:
                df_mod['rmse'] = np.mean(np.array(rmse)[[1, 4, 9, 10, 11, 12, 13]])
            else:
                df_mod['rmse'] = rmse[id]

            if j == 0:
                df_d = df_mod
            else:
                df_d = pd.concat([df_d, df_mod], axis=0)
            print(j)            
            df_mod.to_csv(os.path.join(pdir,f"data_{nmax}_{ai}_{j}_{vara}.csv"))    



# df_rep.psi_leaf_50_close= np.linspace(-1.2, -0.8, nrows)

# first_row = df.iloc[[0]] 
# df_rep= pd.concat([first_row] * nrows, ignore_index=True)
# df_rep['psi88_xylem'] = df_rep['psi50_xylem'] -  np.linspace(0.3, 1.0, nrows)


# first_row = df.iloc[[0]]
# df_rep= pd.concat([first_row] * nrows, ignore_index=True)
# print(df_rep['k_xylem_sat']*18/1000 )
# df_rep['k_xylem_sat'] = 1000/18* np.linspace(0.8, 1.5, nrows)

# first_row = df.iloc[[0]]
# df_rep= pd.concat([first_row] * nrows, ignore_index=True)
# print(df_rep['huber_value'])
# df_rep['huber_value'] = np.linspace(4.9, 5.8, nrows) / 10000.0


# first_row = df.iloc[[0]]
# df_rep= pd.concat([first_row] * nrows, ignore_index=True)
# print(df_rep['stem_hydraulic_capacitance']*18/1000 )
# df_rep['stem_hydraulic_capacitance'] = 1000/18* np.linspace(150, 220, nrows)

# first_row = df.iloc[[30]]
# df_rep= pd.concat([first_row] * nrows, ignore_index=True)
# print(df_rep['stem_hydraulic_capacitance_res']*18/1000 )
# df_rep['stem_hydraulic_capacitance_res'] = 1000/18* np.linspace(10,30, nrows)


# first_row = df.iloc[[0]]
# df_rep= pd.concat([first_row] * nrows, ignore_index=True)
# print(df_rep['psi_tlp'] )
# df_rep['psi_tlp'] = np.linspace(-1.7,-2.3, nrows)