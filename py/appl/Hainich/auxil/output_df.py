import pandas as pd
import numpy as np
import datetime
def create_output_df(output, date_start_str):

    times = np.array(output.Get_times())
    times -= times[0]

    df = pd.DataFrame(times, columns=['DeltaT'])
    offset_date = datetime.datetime.strptime(date_start_str, '%Y-%m-%d %H:%M:%S')
    df['date'] = [offset_date + datetime.timedelta(seconds=int(i)) for i in df['DeltaT'].values]
    df.reset_index()
    df.set_index('date')

    df['vpd'] = output.Get_vpd()
    df['anet'] = output.Get_anet()

    df['ksSoilUp'] = np.array(output.Get_ks_soil())[:, 0];
    df['ksSoil2'] = np.array(output.Get_ks_soil())[:, 1];
    df['ksSoil3'] = np.array(output.Get_ks_soil())[:, 2];

    df['psiSoilUp'] = np.array(output.Get_psi_soil_indiv())[:, 0];
    df['psiStem'] = output.Get_psi_stem()
    df['psiLeaf'] = output.Get_psi_leaf()

    df['T'] = output.Get_T()
    df['G'] = output.Get_G()
    df['J'] = output.Get_J()
    df['J_apdated'] = output.Get_J_per_area()

    df['Gs'] = output.Get_G_per_sap()
    df['Js'] = output.Get_J_per_sap()

    df['gss'] = output.Get_gs()

    df.set_index('date', inplace=True)

    return df