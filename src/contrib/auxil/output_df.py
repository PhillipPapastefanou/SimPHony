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

    nsoil_layers =  np.array(output.Get_ks_soil()).shape[1]

    for sl in range(0,nsoil_layers):
        df[f'ksSoil{sl}'] = np.array(output.Get_ks_soil())[:, sl];

    df['psiSoilUp'] = np.array(output.Get_psi_soil_indiv())[:, 0];
    df['psiSap'] = output.Get_psi_sap()
    df['psiHeart'] = output.Get_psi_heart()
    df['psiLeaf'] = output.Get_psi_leaf()

    df['T'] = output.Get_T()
    df['G'] = output.Get_G()
    df['J'] = output.Get_J()
    df['O'] = output.Get_O()
    df['J_apdated'] = output.Get_J_per_area()

    df['Gs'] = output.Get_G_per_sap()
    df['Js'] = output.Get_J_per_sap()

    df['gss'] = output.Get_gs()

    df['beta'] = output.Get_beta()
    df['anet'] = output.Get_anet()

    df.set_index('date', inplace=True)

    return df