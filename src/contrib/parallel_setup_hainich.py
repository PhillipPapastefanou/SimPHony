import os
import numpy as np
import pandas as pd
import mpi4py.MPI as MPI
import xarray as xr
from time import perf_counter
from src.contrib.config import Config

from SimPHony import Simulation_Multi_Hainich
from SimPHony import DateTime

class ParallelSetupHainich:
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

        # Get the length of the paramter input files
        df = pd.read_csv(self.config.parameter_input_file_list)
        n = df.shape[0]

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

        for i in range(0, self.size):
            df_i = df.iloc[ri: ri + self.n_array_per_process[i]]
            df_i.to_csv(f"{self.config.parameter_input_file_list}{i}")
            self.displ[i] = ri
            ri += self.n_array_per_process[i]

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

        self.comm.Barrier()


    def start_simulations(self):

        if self.is_root:
            print("Starting simulations...", end = '')
            t1 = perf_counter()

        self.sim = Simulation_Multi_Hainich()
        self.sim.Init_Full_Parameter_Setups(f"{self.config.parameter_input_file_list}{self.rank}", np.arange(0,self.n_sims_per_process))
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

        if self.is_root:
            print("Sending data to root...", end = '')
            t1 = perf_counter()

        # Send the RMSE datasets
        nx = self.n_sims_per_process

        data_to_send = np.zeros((nx, 1), dtype='d')
        for i in range(0, nx):
            data_to_send[i] = analysis[i].Get_Rmse_J()
        gathered_data_rmse_J = self._receive_2D_data(nx, 1, data_to_send)
        self.comm.Barrier()

        data_to_send = np.zeros((nx, 1), dtype='d')
        for i in range(0, nx):
            data_to_send[i] = analysis[i].Get_Rmse_G()
        gathered_data_rmse_G = self._receive_2D_data(nx, 1, data_to_send)
        self.comm.Barrier()

        data_to_send = np.zeros((nx, 1), dtype='d')
        for i in range(0, nx):
            data_to_send[i] = analysis[i].Get_Rmse_psi_stem()
        gathered_data_rmse_psi_stem = self._receive_2D_data(nx, 1, data_to_send)
        self.comm.Barrier()

        # # Send the minimum values
        # ny_slices = len(time_slices)
        # data_to_send = np.zeros((nx, ny_slices), dtype='d')
        # for i in range(0, nx):
        #     slices = analysis[i].Get_time_slices()
        #     for j in range (0, ny_slices):
        #         data_to_send[i][j] = slices[j].Min
        # gathered_data_slices = self._receive_2D_data(nx, ny_slices, data_to_send)
        #self.comm.Barrier()

        if self.is_root:
            t2 = perf_counter()
            print(f"Done ({np.round(t2-t1, 1)}) sec.")

        if self.is_root:
            # ds = xr.DataArray(recvbuf2, coords=[('run_id', np.arange(0, sum(count))), ('tree_rmse_id', np.arange(0, ndata_pts_y))])
            print("Writing netcdf file...", end='')
            t1 = perf_counter()
            ds = xr.Dataset(
                {"RMSE_J": (("run_id"), np.squeeze(gathered_data_rmse_J)),
                 "RMSE_G": (("run_id"), np.squeeze(gathered_data_rmse_G)),
                 "RMSE_psi_stem": (("run_id"), np.squeeze(gathered_data_rmse_psi_stem))
                    #,"Minimum": (("run_id", "slices_id"), gathered_data_slices)
                 },
                coords={
                    "run_id": np.arange(0, sum(self.n_array_per_process)),
                    #"rmse": np.arange(0, ny_rmse),
                    #"slices_id": np.arange(0, ny_slices),
                },
            )

            os.makedirs(self.config.output_path, exist_ok=True)

            ds.to_netcdf(f'{self.config.output_path}/Multi_Output{self.rank}.nc',
                         encoding={"RMSE_J": {"dtype": "single"},
                                   "RMSE_G": {"dtype": "single"},
                                   "RMSE_psi_stem": {"dtype": "single"}})
            t2 = perf_counter()
            print(f"Done ({np.round(t2 - t1, 1)}) sec.")


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