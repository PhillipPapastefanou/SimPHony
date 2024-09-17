import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np
import pandas as pd

def std_plot(df : pd.DataFrame, timebegin, timeend, path):
    formatter = mdates.DateFormatter('%m-%d %H');
    df_slice = df.loc[timebegin: timeend]
    fig = plt.figure(figsize=(10, 10))

    ax = fig.add_subplot(3, 2, 1)
    ax.plot(df_slice['vpd'], label='VPD', c='tab:red')
    ax.legend()
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)
    ax.set_ylabel("Vapor pressure deficit [kPa]")

    ax = fig.add_subplot(3, 2, 2)
    ax.plot(df_slice['anet'], label='Anet')
    ax.legend()
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.set_ylabel("Net photosynthesis [mol m-2 s-1]")
    ax.xaxis.set_major_formatter(formatter)

    ax = fig.add_subplot(3, 2, 3)
    ax.semilogy(df_slice['ksSoilUp'], label='ksoil1')
    ax.semilogy(df_slice['ksSoil2'], label='ksoil2')
    ax.semilogy(df_slice['ksSoil3'], label='ksoil3')
    ax.legend()
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    ax = fig.add_subplot(3, 2, 4)
    ax.plot(df_slice['psiLeaf'], label='Leaf')
    ax.plot(df_slice['psiStem'], label='Stem')
    ax.plot(df_slice['psiSoilUp'], label='SoilT')
    ax.legend()
    ax.set_ylabel("Water potential [MPa]")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    ax = fig.add_subplot(3, 2, 5)
    ax.plot(df_slice['T'], label='T', c='tab:blue')
    ax.plot(df_slice['J'], label='J', c='tab:orange')
    ax.plot(df_slice['G'], label='G', c='black')
    ax.legend()
    ax.set_ylabel("Water flows")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)
    ax.set_ylabel("Water fluxes [mol m-2 s-1]")

    ax = fig.add_subplot(3, 2, 6)
    ax.plot(df_slice['gss'], label='gs', c='tab:blue')
    ax.legend()
    ax.set_ylabel("gs")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    plt.subplots_adjust(hspace=0.5, bottom=0.2)
    plt.tight_layout()
    plt.savefig(f"{path}", dpi=300)


def eval_plot(df : pd.DataFrame, df_sap, df_psi_stem, analysis, path):
    formatter = mdates.DateFormatter('%m-%d %H');

    df_slice = df.loc['2023-07-09': '2023-07-27']
    df_slice2 = df.loc['2023-05-01': '2023-11-01']
    fig = plt.figure(figsize=(10, 10))

    ax = fig.add_subplot(2, 2, 1)
    ax.plot(df_slice['vpd'], label='VPD', c='tab:red')
    ax.legend()
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    ax = fig.add_subplot(2, 2, 2)
    ax.plot(df_slice['anet'], label='Anet')
    ax.legend()
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    ax = fig.add_subplot(2, 2, 3)
    ax.plot(df_slice['psiStem'], label='stem_mod', c='tab:red')
    ax.scatter(df_psi_stem['time'], df_psi_stem['psi_stem_obs'], label='stem_obs', s=12, c='black')
    ax.text(0.95, 0.05, f"RMSE: {np.round(analysis.Get_Rmse_psi_stem(),4)}",
        verticalalignment ='bottom',
        horizontalalignment ='right',
        transform = ax.transAxes)
    ax.text(0.95, 0.15, f"LL: {np.round(analysis.Get_LL_psi_stem(),4)}",
        verticalalignment ='bottom',
        horizontalalignment ='right',
        transform = ax.transAxes)
    ax.legend()
    ax.set_ylabel("Water potentials [MPa]")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    ax = fig.add_subplot(2, 2, 4)
    ax.plot(df_slice2['J'], label='J_mod', c='tab:red')
    ax.plot(df_sap['datetime'], df_sap['J'], label='J_obs', c='black', alpha = 0.5)
    ax.text(0.05, 0.95, f"RMSE: {np.round(analysis.Get_Rmse_J(),7)}",
        verticalalignment ='bottom',
        horizontalalignment ='left',
        transform = ax.transAxes)
    ax.text(0.05, 0.85, f"LL: {np.round(analysis.Get_LL_J(),4)}",
        verticalalignment ='bottom',
        horizontalalignment ='left',
        transform = ax.transAxes)
    ax.legend()
    ax.set_ylabel("Water flows")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)



    plt.subplots_adjust(hspace=0.5, bottom=0.2)
    plt.tight_layout()
    plt.savefig(f"{path}", dpi=300)

def eval_plot_24(df : pd.DataFrame, df_sap, df_psi_stem, analysis, path):
    formatter = mdates.DateFormatter('%m-%d %H');

    df_slice = df.loc['2023-07-09': '2023-07-27']
    df_slice2 = df.loc['2023-05-01': '2023-11-01']
    fig = plt.figure(figsize=(10, 10))

    ax = fig.add_subplot(2, 2, 1)
    ax.plot(df_slice['psiStem'], label='stem_mod', c='tab:red')
    ax.scatter(df_psi_stem['time'], df_psi_stem['psi_stem_obs'], label='stem_obs', s=12, c='black')
    ax.text(0.95, 0.05, f"RMSE: {np.round(analysis.Get_Rmse_psi_stem(),4)}",
        verticalalignment ='bottom',
        horizontalalignment ='right',
        transform = ax.transAxes)
    ax.text(0.95, 0.15, f"LL: {np.round(analysis.Get_LL_psi_stem(),4)}",
        verticalalignment ='bottom',
        horizontalalignment ='right',
        transform = ax.transAxes)
    ax.legend()
    ax.set_ylabel("Water potentials [MPa]")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)


    df_slice_group = df_slice.groupby([df_slice.index.hour]).mean()
    df_psi_stem.set_index(df_psi_stem['time'], inplace=True)
    df_psi_stem_group = df_psi_stem.groupby([df_psi_stem.index.hour]).mean()

    ax = fig.add_subplot(2, 2, 2)
    ax.plot(np.arange(0,24),df_slice_group['psiStem'], label='stem_mod', c='tab:red')
    ax.plot(np.arange(0,24),df_psi_stem_group['psi_stem_obs'], label='stem_obs', c='black')
    ax.legend()
    ax.set_ylabel("Water potentials [MPa]")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    #ax.xaxis.set_major_formatter(formatter)

    ax = fig.add_subplot(2, 2, 3)
    ax.plot(df_slice2['J'], label='J_mod', c='tab:red')
    ax.plot(df_sap['datetime'], df_sap['J'], label='J_obs', c='black', alpha = 0.5)
    ax.text(0.05, 0.95, f"RMSE: {np.round(analysis.Get_Rmse_J(),7)}",
        verticalalignment ='bottom',
        horizontalalignment ='left',
        transform = ax.transAxes)
    ax.text(0.05, 0.85, f"LL: {np.round(analysis.Get_LL_J(),4)}",
        verticalalignment ='bottom',
        horizontalalignment ='left',
        transform = ax.transAxes)
    ax.legend()
    ax.set_ylabel("Water flows")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)
    ax.xaxis.set_major_formatter(formatter)

    df_sap.set_index(df_sap['datetime'], inplace=True)
    df_sap_group = df_sap.groupby([df_sap.index.hour, df_sap.index.minute]).mean()

    #df_slice2.set_index(df_slice2['datetime'], inplace=True)
    df_slice2_group = df_slice2.groupby([df_slice2.index.hour, df_slice2.index.minute]).mean()

    h = df_slice2_group['J'].astype(float).to_frame()
    i = df_sap_group['J'].astype(float).to_frame()
    ax = fig.add_subplot(2, 2, 4)
    ax.plot(np.arange(0,24, 0.5),h['J'].values, label='J_mod', c='tab:red')
    ax.plot(np.arange(0, 24, 0.5), i['J'].values, label='J_obs', c='black', alpha = 0.5)
    ax.text(0.05, 0.95, f"RMSE: {np.round(analysis.Get_Rmse_J(),7)}",
        verticalalignment ='bottom',
        horizontalalignment ='left',
        transform = ax.transAxes)
    ax.text(0.05, 0.85, f"LL: {np.round(analysis.Get_LL_J(),4)}",
        verticalalignment ='bottom',
        horizontalalignment ='left',
        transform = ax.transAxes)
    ax.legend()
    ax.set_ylabel("Water flows")
    ax.set_xlabel("Time")
    ax.tick_params(axis='x', labelrotation=45)



    plt.subplots_adjust(hspace=0.5, bottom=0.2)
    plt.tight_layout()
    plt.savefig(f"{path}", dpi=300)