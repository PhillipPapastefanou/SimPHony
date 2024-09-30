# Importing baseline libraries
import sys
import os
import numpy as np
import pandas as pd
import datetime
import copy
import matplotlib.dates as mdates

# Specify paths so that the PHS model can be imported
root_library_path = '/Users/pp/Documents/Repos/plant_hydro_standalone'
root_data_path = os.path.join(root_library_path, 'data')
cpp_lib_path = os.path.join(root_library_path, 'cpp', 'cmake-build-release')
sys.path.append(cpp_lib_path)

# Importing local libraries and paths
from hydro_standalone import Simulation_Single_Hainich
from hydro_standalone import DateTime
from src.Parameters import Parameters
from src.Parameters import SoilLayer
from src.Parameters import Soil_Water_Model_Type
from src.Parameters import Convert_Soil_Parameters
from contrib.parameter_parser import Parameter_Parser

from appl.Hainich.auxil.output_df import create_output_df
from appl.Hainich.auxil.output_plotter import std_plot
from appl.Hainich.auxil.output_plotter import eval_plot
from appl.Hainich.auxil.output_plotter import eval_plot_24

# Specifying forcing and evalution data paths
forcing_file = os.path.join(root_data_path, 'hainich', 'input', 'Meteo_Hainich_dT30min_forcing_PHS.csv')
sapflux_file = os.path.join(root_data_path, 'hainich', 'eval', 'SAP_Hainich_Fagus-mean_dT30min_prog.csv')
psi_stem_file = os.path.join(root_data_path, 'hainich', 'eval', 'stem_water_pot.csv')

root_output_directory = "/Users/pp/data/Simulations/A08_Hydraulics_standalone/hainich"
scenario_name = "fine_vg_more_constrained"
input_path = os.path.join(root_output_directory,scenario_name,'input')
output_path = os.path.join(root_output_directory,scenario_name,'output')
post_path = os.path.join(root_output_directory,scenario_name,'post')


#parameters_list = os.path.join(input_path, 'parameters_best_psi_stem.csv')
#parameters_list = os.path.join(input_path, 'parameters_best_J.csv')

# Reading sapflow and psi_stem data
df_sap_obs = pd.read_csv(sapflux_file)
df_sap_obs['datetime']  = pd.to_datetime(df_sap_obs['datetime'])
df_psi_stem_obs = pd.read_csv(psi_stem_file)
df_psi_stem_obs['time']  = pd.to_datetime(df_psi_stem_obs['time'])
# Convert from kg H2O to mol H2O
df_psi_stem_obs['psi_stem_obs'] = df_psi_stem_obs['FAG']



optimizations = ['both', 'psi_stem', 'J']

for opt in optimizations:

    path_output_loc = os.path.join(post_path, opt)
    os.makedirs(path_output_loc, exist_ok=True)

    parameters_list = os.path.join(post_path, f'df_{opt}_sel_ordered.csv')

    # ------------------------------------------------------
    # Parameter setup
    # ------------------------------------------------------

    for i in range(0, 1000):
        parameter_id = i

        # ----------------------------------------------------------------
        # PHS model simulation
        # ----------------------------------------------------------------
        # Set up the PHS simulation
        # read in the parameter file that we just created
        sim = Simulation_Single_Hainich()
        sim.Init_parameters_fn_single(parameters_list, parameter_id)
        sim.Init_input(forcing_file, sapflux_file, psi_stem_file)
        sim.Set_water_pot_initials(-1.0, -0.2)

        # Specify Start and End of the Simulation
        format = "%Y-%m-%d %H:%M:%S"
        date_start_str = "2023-04-01 00:00:00"
        date_end_str = "2023-11-01 00:00:00"
        timebegin = DateTime(date_start_str, format)
        timeend   = DateTime(date_end_str, format)

        # Run the simulation
        sim.Run(timebegin, timeend)

        # Get output and analysis data
        output = sim.Get_output()
        an = sim.Get_analysis()

        # Create the main output from the analysis
        df = create_output_df(output, date_start_str)


        df['id'] = int(i)

        if i == 0:
            df_d = df
        else:
            df_d = pd.concat([df_d, df], axis=0)
        print(i)


    print(df_d['id'].describe())



    import matplotlib.pyplot as plt
    import matplotlib.dates as mdates


    formatter = mdates.DateFormatter('%m-%d %H');


    begin, end = '2023-07-09', '2023-07-27'



    dm = df_d.groupby(df_d.index).median()
    d25 = df_d.groupby(df_d.index).quantile(0.25)
    d75 = df_d.groupby(df_d.index).quantile(0.75)

    dm_slice = dm.loc[begin : end]
    d25_slice = d25.loc[begin : end]
    d75_slice = d75.loc[begin : end]

    df_sap_obs.set_index(df_sap_obs['datetime'], inplace=True)
    df_sap_obs = df_sap_obs.loc[begin: end]

    fig = plt.figure(figsize=(10, 10))
    ax = fig.add_subplot(2, 2, 1)
    ax.plot(dm_slice['psiStem'], label='stem_mod', c='tab:red')
    ax.fill_between(d25_slice.index, d25_slice['psiStem'], d75_slice['psiStem'], label='stem_mod',
                    color='tab:red', alpha=0.4)
    #ax.plot(d75_slice['psiStem'], label='stem_mod', c='tab:green')
    ax.scatter(df_psi_stem_obs['time'], df_psi_stem_obs['psi_stem_obs'], label='stem_obs', s=12, c='black')

    ax.legend()
    ax.set_ylabel("Water potentials [MPa]")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    df_u = dm_slice.copy()
    df_u.reset_index(inplace=True)
    df_u['time'] = df_u['date']
    df_x = pd.merge(df_u, df_psi_stem_obs, on='time')
    df_x.set_index('time', inplace=True)
    df_x_group_m = df_x.groupby([df_x.index.hour]).mean()

    df_u = d25_slice.copy()
    df_u.reset_index(inplace=True)
    df_u['time'] = df_u['date']
    df_x = pd.merge(df_u, df_psi_stem_obs, on='time')
    df_x.set_index('time', inplace=True)
    df_x_group_25 = df_x.groupby([df_x.index.hour]).mean()

    df_u = d75_slice.copy()
    df_u.reset_index(inplace=True)
    df_u['time'] = df_u['date']
    df_x = pd.merge(df_u, df_psi_stem_obs, on='time')
    df_x.set_index('time', inplace=True)
    df_x_group_75 = df_x.groupby([df_x.index.hour]).mean()

    ax = fig.add_subplot(2, 2, 2)
    ax.plot(np.arange(0, 24), df_x_group_m['psiStem'], label='stem_mod', c='tab:red')

    ax.fill_between(np.arange(0, 24), df_x_group_25['psiStem'], df_x_group_75['psiStem'],
                    label='stem_mod', color='tab:red', alpha=0.4)
    ax.plot(np.arange(0, 24), df_x_group_m['psi_stem_obs'], label='stem_obs', c='black')
    ax.legend()
    ax.set_ylabel("Water potentials [MPa]")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
# ax.xaxis.set_major_formatter(formatter)

    ax = fig.add_subplot(2, 2, 3)
    ax.plot(dm_slice['J_apdated'], label='stem_mod', c='tab:red')
    ax.fill_between(d25_slice.index, d25_slice['J_apdated'], d75_slice['J_apdated'], label='stem_mod',
                    color='tab:red', alpha=0.4)
    ax.plot(df_sap_obs['datetime'], df_sap_obs['J'], label='J_obs', c='black', alpha=0.5)
    ax.legend()
    ax.set_ylabel("Water flows")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    df_sap_group = df_sap_obs.groupby([df_sap_obs.index.hour, df_sap_obs.index.minute]).mean()

    # df_slice2.set_index(df_slice2['datetime'], inplace=True)
    #df_slice2_group = df_slice.groupby([df_slice.index.hour, df_slice.index.minute]).mean()

    #h = df_slice2_group['J_apdated'].astype(float).to_frame()
    i = df_sap_group['J'].astype(float).to_frame()
    ax = fig.add_subplot(2, 2, 4)




    dm_slice_group = dm_slice.groupby([dm_slice.index.hour, dm_slice.index.minute]).mean()
    dm_slice_group = dm_slice_group['J_apdated'].astype(float).to_frame()

    d25_slice_group = d25_slice.groupby([d25_slice.index.hour, d25_slice.index.minute]).mean()
    d25_slice_group = d25_slice_group['J_apdated'].astype(float).to_frame()

    d75_slice_group = d75_slice.groupby([d75_slice.index.hour, d75_slice.index.minute]).mean()
    d75_slice_group = d75_slice_group['J_apdated'].astype(float).to_frame()


    ax.plot(np.arange(0, 24, 0.5), dm_slice_group['J_apdated'].values, label='J_mod', c='tab:red')
    ax.fill_between(np.arange(0, 24, 0.5), d25_slice_group['J_apdated'], d75_slice_group['J_apdated'], label='stem_mod',
                    color='tab:red', alpha=0.4)


    ax.plot(np.arange(0, 24, 0.5), i['J'].values, label='J_obs', c='black', alpha=0.5)

    ax.legend()
    ax.set_ylabel("Water flows")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)

    plt.subplots_adjust(hspace=0.5, bottom=0.2)
    plt.tight_layout()
    plt.savefig(f"{path_output_loc}/best_selection.png", dpi=300)


    variables = ['psiLeaf', 'anet', 'Gs', 'gss', 'T']
    for var in variables:
        fig = plt.figure(figsize=(10, 10))
        ax = fig.add_subplot(1, 2, 1)
        ax.plot(dm_slice[var], c='tab:red')
        ax.fill_between(d25_slice.index, d25_slice[var], d75_slice[var], label='stem_mod',
                        color='tab:red', alpha=0.4)

        ax.legend()
        ax.set_ylabel("Water potentials [MPa]")
        ax.set_xlabel("Time")
        ax.tick_params(axis='x', labelrotation=45)
        ax.xaxis.set_major_formatter(formatter)

        df_u = dm_slice.copy()
        df_u.reset_index(inplace=True)
        df_u['time'] = df_u['date']
        df_x = pd.merge(df_u, df_psi_stem_obs, on='time')
        df_x.set_index('time', inplace=True)
        df_x_group_m = df_x.groupby([df_x.index.hour]).mean()

        df_u = d25_slice.copy()
        df_u.reset_index(inplace=True)
        df_u['time'] = df_u['date']
        df_x = pd.merge(df_u, df_psi_stem_obs, on='time')
        df_x.set_index('time', inplace=True)
        df_x_group_25 = df_x.groupby([df_x.index.hour]).mean()

        df_u = d75_slice.copy()
        df_u.reset_index(inplace=True)
        df_u['time'] = df_u['date']
        df_x = pd.merge(df_u, df_psi_stem_obs, on='time')
        df_x.set_index('time', inplace=True)
        df_x_group_75 = df_x.groupby([df_x.index.hour]).mean()

        ax = fig.add_subplot(1, 2, 2)
        ax.plot(np.arange(0, 24), df_x_group_m[var], c='tab:red')

        ax.fill_between(np.arange(0, 24), df_x_group_25[var], df_x_group_75[var],
                        label='stem_mod', color='tab:red', alpha=0.4)
        ax.legend()
        ax.set_ylabel("Water potentials [MPa]")
        ax.set_xlabel("Time")
        ax.tick_params(axis='x', labelrotation=45)

        plt.subplots_adjust(hspace=0.5, bottom=0.2)
        plt.tight_layout()
        plt.savefig(f"{path_output_loc}/dist_{var}.png", dpi=300)
        # ax.xaxis.set_major_formatter(formatter)