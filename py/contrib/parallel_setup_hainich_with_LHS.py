import numpy as np
import pandas as pd
import os
import mpi4py.MPI as MPI
import xarray as xr
from time import perf_counter
from hydro_standalone import Simulation_Multi_Hainich
from hydro_standalone import DateTime
from contrib.config import Config
#from appl.param_generation.LHS.LHS_Multi_Hainich_Application_automation_campbell import Calculate_LHS_per_process
from appl.param_generation.LHS.LHS_Multi_Hainich_Application_automation_vangenuchten import Calculate_LHS_per_process
class ParallelSetupHainichWithLHS:
    def __init__(self, comm, rank, size):

        self.comm = comm
        self.size = size
        self.rank = rank
        self.is_root = rank == 0

    def init(self, config : Config):
        self.config = config
        if self.is_root:
            self._calculate_gridpoints()
        else:
            self._initialise_counts()

    def _calculate_gridpoints(self):

        n = self.config.nsims

        min_np = int(n / self.size)
        remaining = n - min_np * self.size
        n_per_process = np.zeros(self.size) + min_np

        offsets = np.zeros(self.size)
        for i in range(0, remaining):
            offsets[i] = 1
        n_per_process  += offsets

        # Initialise inter with array as send an int is not possible atm
        self.n_sims_total = np.arange(1).astype(int)
        self.n_sims_total[0] = n

        self.n_array_per_process = n_per_process.astype(int)

        ri = 0
        self.displ = np.zeros(self.size)

        # for i in range(0, self.size):
        #     df_i = df.iloc[ri: ri + self.n_array_per_process[i]]
        #     df_i.to_csv(f"{self.config.parameter_input_file_list}{i}")
        #     self.displ[i] = ri
        #     ri += self.n_array_per_process[i]

    def _initialise_counts(self):
        self.sendbuf = None
        # initialize count on worker processes
        self.n_array_per_process = np.zeros(self.size, dtype=int)
        self.n_sims_total =  np.zeros(1, dtype=int)
        self.displ = None

    def send_parameter_indexes(self):

        if self.is_root:
            print("Broadcasting parameter indices...", end = '')
            t1 = perf_counter()

        # broadcast The number of parameter files each process will get
        self.comm.Bcast(self.n_sims_total, root=0)
        self.comm.Bcast(self.n_array_per_process, root=0)
        self.n_sims_per_process = self.n_array_per_process[self.rank]

        # Print the chunk that was received by this process
        print("Process {} received chunk {}".format(self.rank, self.n_sims_per_process))
        print(self.n_sims_total)

        # Create directory
        if self.rank == 0:
            if not os.path.exists(self.config.input_path):
                os.makedirs(self.config.input_path)
            if not os.path.exists(self.config.output_path):
                os.makedirs(self.config.output_path)
            if not os.path.exists(self.config.post_path):
                os.makedirs(self.config.post_path)
        self.comm.Barrier()

        # Create LHS parameter setup
        Calculate_LHS_per_process(self.rank, self.n_sims_per_process, self.config.input_path)

    def start_simulations(self):

        if self.is_root:
            print("Starting simulations...", end = '')
            t1 = perf_counter()

        self.sim = Simulation_Multi_Hainich()
        self.sim.Init_Full_Parameter_Setups(f"{self.config.input_path}/Hainich_parameters.csv{self.rank}",
                                            np.arange(0,self.n_sims_per_process))
        self.sim.Init_input(self.config.forcing_file,
                            self.config.sap_file,
                            self.config.psi_stem_file,
                            self.rank)

        self.sim.Set_water_pot_initials(-1.0, -0.2)

        # Step length should be 30 mins
        format = "%Y-%m-%d %H:%M:%S"
        timestart = DateTime("2023-04-01 00:00:00", format)
        timeend   = DateTime("2023-11-01 00:00:00", format)

        self.sim.Run(timestart, timeend)
        self.comm.Barrier()

        if self.is_root:
            t2 = perf_counter()
            print(f"Done ({np.round(t2 - t1, 1)}) sec.")

    def receive_analysis_data(self):

        analysis = self.sim.Get_analysis_list()

        nx = self.n_sims_per_process

        rmse_J = np.zeros(nx)
        rmse_G = np.zeros(nx)
        rmse_psi_stem = np.zeros(nx)

        for i in range(0, nx):
            rmse_J[i] = analysis[i].Get_Rmse_J()
            rmse_G[i] = analysis[i].Get_Rmse_G()
            rmse_psi_stem[i] = analysis[i].Get_Rmse_psi_stem()

        df_rmse = pd.DataFrame()
        df_rmse['rmse_J'] = rmse_J
        df_rmse['rmse_G'] = rmse_G
        df_rmse['rmse_psi_stem'] = rmse_psi_stem

        df_input = pd.read_csv(f"{self.config.input_path}/Hainich_parameters.csv{self.rank}")

        df_input['psi_soil_sat0'] = df_input['psi_soil_sats'].str.split(';', expand=True).values[:, 0].astype(float)
        df_input['k_soil_sat0'] = df_input['k_soil_sats'].str.split(';', expand=True).values[:, 0].astype(float)
        df_input['theta_s0'] = df_input['theta_s'].str.split(';', expand=True).values[:, 0].astype(float)
        df_input['theta_r0'] = df_input['theta_r'].str.split(';', expand=True).values[:, 0].astype(float)
        df_input['pore_0'] = df_input['pore_size_ind'].str.split(';', expand=True).values[:, 0].astype(float)

        df_c = pd.concat([df_input, df_rmse], axis=1)

        max_rmse_J = 0.000347
        max_rmse_psi_stem = 0.0668
        df_c['rmse_com_avg'] = (df_c['rmse_psi_stem'] / max_rmse_psi_stem + df_c['rmse_J'] / max_rmse_J) / 2.0;

        output_path = self.config.output_path
        nbest = self.config.nbest
        df_best_psi_stem = df_c.sort_values(by='rmse_psi_stem').iloc[0:nbest]
        df_best_psi_stem.to_csv(f"{output_path}/parameters_best_psi_stem.csv{self.rank}")
        df_best_J = df_c.sort_values(by='rmse_J').iloc[0:nbest]
        df_best_J.to_csv(f"{output_path}/parameters_best_J.csv{self.rank}")
        df_best_both = df_c.sort_values(by='rmse_com_avg').iloc[0:nbest]
        df_best_both.to_csv(f"{output_path}/parameters_best_both.csv{self.rank}")

        self.comm.Barrier()

        if self.is_root:
            for str in ['psi_stem','J', 'both']:
                df_0 = pd.read_csv(f"{output_path}/parameters_best_{str}.csv{0}")
                for i in range(1, self.size):
                    df = pd.read_csv(f"{output_path}/parameters_best_{str}.csv{i}")
                    df_0 = pd.concat([df_0, df], axis = 0)
                df_0.to_csv(f"{self.config.post_path}/parameters_best_{str}.csv")

    def _receive_2D_data(self, nx, ny, data_to_send):

        count_transfer = self.n_array_per_process * nx
        sendbuf = data_to_send
        recvbuf = np.zeros((sum(self.n_array_per_process), ny), dtype='d')

        if self.is_root:
            displ = np.copy(self.displ)
            displ *= ny
        else:
            displ = self.displ

        print(f"Rank {self.rank} count {self.n_array_per_process}")
        print(f"Rank {self.rank} disp {self.displ}")

        self.comm.Gatherv(sendbuf, [recvbuf, count_transfer,  displ, MPI.DOUBLE], root=0)
        # if self.is_root == 0:
        #     print('After Gatherv, process 0 has data:', recvbuf)
        return recvbuf