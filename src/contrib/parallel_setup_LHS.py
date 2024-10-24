import numpy as np
import pandas as pd
import os
import mpi4py.MPI as MPI
import xarray as xr
from time import perf_counter
from SimPHony import Simulation_Multi_Hainich
from SimPHony import Simulation_Multi_Swiss
from SimPHony import DateTime
from src.contrib.config import Config, Location

from src.contrib.param_generation.LHS.LHS_Multi_Swiss_Application_automation_vangenuchten import Calculate_LHS_per_process_swiss_cc
from src.contrib.param_generation.LHS.LHS_Multi_Hainich_Application_automation_vangenuchten import Calculate_LHS_per_process_hainich

class ParallelSetupWithLHS:
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

        # Create directory
        if self.rank == 0:
            if not os.path.exists(self.config.input_path):
                os.makedirs(self.config.input_path)
            if not os.path.exists(self.config.output_path):
                os.makedirs(self.config.output_path)
            if not os.path.exists(self.config.post_path):
                os.makedirs(self.config.post_path)
        self.comm.Barrier()

        print("Calculating parameter setup...")
        # Create LHS parameter setup
        if self.config.location == Location.Hainich:
            Calculate_LHS_per_process_hainich(self.rank, self.n_sims_per_process, self.config.input_path)
        elif self.config.location == Location.Swiss:
            Calculate_LHS_per_process_swiss_cc(self.rank, self.n_sims_per_process, self.config.input_path)
        else:
            print("Invalid location specified. Exiting...")
            exit(99)
        print("Done.")

    def start_simulations(self):

        DATE_FORMAT = "%Y-%m-%d %H:%M:%S"
        
        if self.is_root:
            t1 = perf_counter()

        if self.config.location == Location.Hainich:
            self.sim = Simulation_Multi_Hainich()
            self.sim.Init_Full_Parameter_Setups(f"{self.config.input_path}/Hainich_parameters.csv{self.rank}",
                                                np.arange(0,self.n_sims_per_process))
            self.sim.Init_input(self.config.forcing_file,
                                self.config.sap_file,
                                self.config.psi_stem_file,
                                self.rank)

            timestart = DateTime("2023-04-01 00:00:00", DATE_FORMAT)
            timeend   = DateTime("2023-11-01 00:00:00", DATE_FORMAT)

        elif self.config.location == Location.Swiss:
            self.sim = Simulation_Multi_Swiss()
            self.sim.Init_Full_Parameter_Setups(f"{self.config.input_path}/Swiss_cc_parameters.csv{self.rank}",
                                                np.arange(0, self.n_sims_per_process))
            self.sim.Init_input(self.config.soilwater_file,
                                self.config.forcing_file,
                                self.config.tree_folder,
                                self.rank)

            timestart = DateTime("2018-05-01 00:00:00", DATE_FORMAT)
            timeend = DateTime("2018-12-15 00:00:00", DATE_FORMAT)
        else:
            print("Invalid location specified. Exiting...")
            exit(99)


        self.sim.Set_water_pot_initials(-1.0, -0.2)

        print(f"Rank {self.rank} is starting SimPHony...")
        self.sim.Run(timestart, timeend)
        print(f"Rank {self.rank} finished simulating.")
        self.comm.Barrier()

        if self.is_root:
            t2 = perf_counter()
            print(f"All simulations completed ({np.round(t2 - t1, 1)}) sec.")

    def receive_analysis_data(self):

        if self.is_root:
            t1 = perf_counter()
            print("Starting analysis...")

        analysis = self.sim.Get_analysis_list()
        nx = self.n_sims_per_process

        if self.config.location == Location.Hainich:
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

        elif self.config.location == Location.Swiss:
            data = np.zeros((nx, 14))
            for i in range(nx):
                data[i] = analysis[i].Get_rmse()

            df_rmse = pd.DataFrame(data)
            df_rmse.columns = [f't{i}' for i in range(14)]

            range_alive = np.array([1, 4, 9, 10, 11, 12, 13])
            data_alive = data.T[range_alive]
            data_alive_mean = np.mean(data_alive, axis=0)
            df_rmse['alive_mean'] = data_alive_mean

            df_input = pd.read_csv(f"{self.config.input_path}/Swiss_cc_parameters.csv{self.rank}")


        df_input['psi_soil_sat0'] = df_input['psi_soil_sats'].str.split(';', expand=True).values[:, 0].astype(float)
        df_input['k_soil_sat0'] = df_input['k_soil_sats'].str.split(';', expand=True).values[:, 0].astype(float)
        df_input['theta_s0'] = df_input['theta_s'].str.split(';', expand=True).values[:, 0].astype(float)
        df_input['theta_r0'] = df_input['theta_r'].str.split(';', expand=True).values[:, 0].astype(float)
        df_input['pore_0'] = df_input['pore_size_ind'].str.split(';', expand=True).values[:, 0].astype(float)

        df_c = pd.concat([df_input, df_rmse], axis=1)

        if self.config.location == Location.Hainich:
            REFERENCE_RMSE_J = 0.000150
            REFERENCE_RMSE_PSI_STEM = 0.0668
            df_c['rmse_com_avg'] = (df_c['rmse_psi_stem'] / REFERENCE_RMSE_PSI_STEM + df_c['rmse_J'] / REFERENCE_RMSE_J) / 2.0;

        output_path = self.config.output_path
        nbest = self.config.nbest

        if self.config.location == Location.Hainich:
            df_best_psi_stem = df_c.sort_values(by='rmse_psi_stem').iloc[0:nbest]
            df_best_psi_stem.to_csv(f"{output_path}/parameters_best_psi_stem.csv{self.rank}")
            df_best_J = df_c.sort_values(by='rmse_J').iloc[0:nbest]
            df_best_J.to_csv(f"{output_path}/parameters_best_J.csv{self.rank}")
            df_best_both = df_c.sort_values(by='rmse_com_avg').iloc[0:nbest]
            df_best_both.to_csv(f"{output_path}/parameters_best_both.csv{self.rank}")

        elif self.config.location == Location.Swiss:
            for i in range(14):
                df_best_psi_stem = df_c.sort_values(by=f't{i}').iloc[0:nbest]
                df_best_psi_stem.to_csv(f"{output_path}/parameters_f{i}.csv{self.rank}")

            df_best_psi_stem = df_c.sort_values(by='alive_mean').iloc[0:nbest]
            df_best_psi_stem.to_csv(f"{output_path}/parameters_alive_mean.csv{self.rank}")

        self.comm.Barrier()

        if self.is_root:
            if self.config.location == Location.Hainich:
                for str in ['psi_stem','J', 'both']:
                    df_0 = pd.read_csv(f"{output_path}/parameters_best_{str}.csv{0}")
                    for i in range(1, self.size):
                        df = pd.read_csv(f"{output_path}/parameters_best_{str}.csv{i}")
                        df_0 = pd.concat([df_0, df], axis = 0)
                    df_0.to_csv(f"{self.config.post_path}/parameters_best_{str}.csv")

            elif self.config.location == Location.Swiss:
                for j in range(14):
                    df_0 = pd.read_csv(f"{output_path}/parameters_f{j}.csv{0}")
                    for i in range(1, self.size):
                        df = pd.read_csv(f"{output_path}/parameters_f{j}.csv{i}")
                        df_0 = pd.concat([df_0, df], axis=0)
                    df_0.to_csv(f"{self.config.post_path}/parameters_best_{j}.csv")

                df_0 = pd.read_csv(f"{output_path}/parameters_alive_mean.csv{0}")
                for i in range(1, self.size):
                    df = pd.read_csv(f"{output_path}/parameters_alive_mean.csv{i}")
                    df_0 = pd.concat([df_0, df], axis=0)
                df_0.to_csv(f"{self.config.post_path}/parameters_best_mean_alive.csv")

            t2 = perf_counter()
            print(f"Analysis completed ({np.round(t2 - t1, 1)}) sec.")

    # def _receive_2D_data(self, nx, ny, data_to_send):
    #
    #     count_transfer = self.n_array_per_process * nx
    #     sendbuf = data_to_send
    #     recvbuf = np.zeros((sum(self.n_array_per_process), ny), dtype='d')
    #
    #     if self.is_root:
    #         displ = np.copy(self.displ)
    #         displ *= ny
    #         displ = displ.astype(int)
    #     else:
    #         displ = self.displ
    #
    #     print(f"Rank {self.rank} count {self.n_array_per_process}")
    #     print(f"Rank {self.rank} disp {self.displ}")
    #
    #     self.comm.Gatherv(sendbuf, [recvbuf, count_transfer,  displ, MPI.DOUBLE], root=0)
    #     # if self.is_root == 0:
    #     #     print('After Gatherv, process 0 has data:', recvbuf)
    #     return recvbuf