
import os
import pandas as pd
import glob
import sys
import importlib
from src.contrib.config import Swiss_soil_water_input_type
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

def listdir_fullpath(d):
    return [os.path.join(d, f) for f in os.listdir(d)]

def get_lib_directory():
    return os.path.join(THIS_DIR, os.pardir, os.pardir, os.pardir)

def get_SimPHony_build_path():
    root_path = get_lib_directory()
    dir_entries = listdir_fullpath(root_path)
    folders = [a for a in dir_entries if os.path.isdir(a)]

    found_lib = False
    lib_path = ""
    lib_folder = ""
    for str_path in folders:

        sub_dir_entries = listdir_fullpath(str_path)
        files = [a for a in sub_dir_entries if os.path.isfile(a)]

        for file in files:
            if (file.endswith(".so")) & (not 'debug' in file):
                found_lib = True
                lib_path = file
                lib_folder = str_path
                break

    return found_lib, lib_path, lib_folder

def get_SimPHony_test_build_path():
    root_path = get_lib_directory()
    dir_entries = listdir_fullpath(root_path)
    folders = [a for a in dir_entries if os.path.isdir(a)]

    found_lib = False
    lib_path = ""
    lib_folder = ""
    for str_path in folders:

        if 'debug' in str_path:
            continue

        sub_dir_entries = listdir_fullpath(str_path)
        files = [a for a in sub_dir_entries if os.path.isfile(a)]

        for file in files:
            fname = file.split(os.sep)[-1]
            if (fname == 'SimPHony_tests') |(fname == 'SimPHony_tests.exe'):
                found_lib = True
                lib_path = file
                lib_folder = str_path
                break

    return found_lib, lib_path, lib_folder

def get_forcing_hainich():
    root_library_path = get_lib_directory()
    root_data_path = os.path.join(root_library_path, 'data')
    file_forcing = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
    df_forcing = pd.read_csv(file_forcing)
    df_forcing['datetime'] = pd.to_datetime(df_forcing['datetime'])
    return file_forcing, df_forcing

# ----------------------------------------------------------
# Hainich setup
# ----------------------------------------------------------


def get_sapflow_filepath_hainich():
    root_library_path = get_lib_directory()
    root_data_path = os.path.join(root_library_path, 'data')
    return os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')

def get_sapflow_data_hainich():
    file_sap_obs = get_sapflow_filepath_hainich()
    df_sap_obs = pd.read_csv(file_sap_obs)
    df_sap_obs['datetime'] = pd.to_datetime(df_sap_obs['datetime'])
    return df_sap_obs

def get_psi_stem_filepath_hainich():
    root_library_path = get_lib_directory()
    root_data_path = os.path.join(root_library_path, 'data')
    return os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')

def get_psi_stem_data_hainich():
    file_psi_stem_obs = get_psi_stem_filepath_hainich()
    df_psi_stem_obs = pd.read_csv(file_psi_stem_obs)
    df_psi_stem_obs['time'] = pd.to_datetime(df_psi_stem_obs['time'])
    # Rename column to match the rest of the scripts
    df_psi_stem_obs['psi_stem_obs'] = df_psi_stem_obs['FAG']
    return df_psi_stem_obs

# ----------------------------------------------------------
# Swiss cc setup
# ----------------------------------------------------------

def get_forcing_filepath_swiss_cc():
    root_library_path = get_lib_directory()
    root_data_path = os.path.join(root_library_path, 'data')
    return os.path.join(root_data_path, 'swiss', 'input', 'Forcing_Inter.csv')

def get_forcing_data_swiss_cc():
    file_forcing = get_forcing_filepath_swiss_cc()
    df_forcing = pd.read_csv(file_forcing)
    df_forcing['dt'] = pd.to_datetime(df_forcing['dt'])
    return df_forcing


def get_soil_water_filepath_swiss_cc(soilwater_input_type : Swiss_soil_water_input_type):
    root_library_path = get_lib_directory()
    root_data_path = os.path.join(root_library_path, 'data')
    if soilwater_input_type == Swiss_soil_water_input_type.NLayers_Indiv:
        file_soil_water = os.path.join(root_data_path, 'swiss', 'input', 'vwc_swicc_cc_2023_indiv.csv')
    elif soilwater_input_type == Swiss_soil_water_input_type.NLayers_Mean_N_Std:
        file_soil_water = os.path.join(root_data_path, 'swiss', 'input', 'vwc_swicc_cc_2023_std_n.csv')
    elif soilwater_input_type == Swiss_soil_water_input_type.NLayers_Mean_One_Std:
        file_soil_water = os.path.join(root_data_path, 'swiss', 'input', 'swiss_cc_soil_water_with_sd.csv')
    else: 
        print("Soil water input file not specified.")
        exit(99)
    return file_soil_water;

def ger_soil_water_data_swiss_cc(soilwater_input_type : Swiss_soil_water_input_type):
    file_soil_water = get_soil_water_filepath_swiss_cc(soilwater_input_type)
    df_soil_water = pd.read_csv(file_soil_water)
    df_soil_water['dates'] = pd.to_datetime(df_soil_water['dates'])
    return file_soil_water, df_soil_water

def get_trees_psi_leaf_folder_path_cc():
    root_library_path = get_lib_directory()
    root_data_path = os.path.join(root_library_path, 'data')
    file_soil_water = os.path.join(root_data_path, 'swiss', 'eval', 'Trees')
    return file_soil_water