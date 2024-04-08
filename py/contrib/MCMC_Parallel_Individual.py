import numpy as np
import pandas as pd
import mpi4py.MPI as MPI
import xarray as xr
from time import perf_counter

from hydro_standalone import Simulation_Multi_Hainich
from hydro_standalone import DateTime
from contrib.config import Config
from numpy import random

class ParallelSetupIndividual:
    def __init__(self, comm, rank, size):

        self.comm = comm
        self.size = size
        self.rank = rank
        self.is_root = rank == 0

        random.seed(seed=123)

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


        df = pd.read_csv(self.config.parameter_input_file_list_partial)
        ri = 0
        for i in range(0, self.size):
            df_i = df.iloc[ri: ri + self.n_array_per_process[i]]
            df_i.to_csv(f"{self.config.parameter_input_file_list_partial}{i}")
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
        self.sim.Init_input(self.config.forcing_file, self.config.sap_file, self.rank)
        self.sim.Set_water_pot_initials(-1.0, -0.3)

        # in seconds
        steplen = 1800

        # Steplenght should be 30 mins
        format = "%Y-%m-%d %H:%M:%S"
        timestart = DateTime("2023-6-1 00:00:00", format)
        timeend = DateTime("2023-10-1 00:00:00", format)

        self.sim.Run(steplen, timestart, timeend)
        self.comm.Barrier()

        if self.is_root:
            t2 = perf_counter()
            print(f"Done ({np.round(t2-t1, 1)}) sec.")


    def perform_analysis(self):

        analysis_list = self.sim.Get_analysis_list()

        t1 = perf_counter()

        # Send the RMSE datasets
        nx = self.n_sims_per_process
        # Apply metropolis hastings algorithm

        full_parameters = pd.read_csv(f"{self.config.parameter_input_file_list_partial}{self.rank}")

        ll_list = []
        parameter_list = []
        ll_list.append(analysis_list[0].Get_LL_J())
        parameter_list.append(full_parameters.iloc[0].values)

        for i in range(1, nx):
            ll =  analysis_list[i].Get_LL_J()

            if ll >= ll_list[-1] + np.log(random.rand()):
                ll_list.append(ll)
                parameter_list.append(full_parameters.iloc[i].values)
            else:
                ll_list.append(ll_list[-1])
                parameter_list.append(parameter_list[-1])

        # data_to_send = np.zeros((nx, 1), dtype='d')
        # for i in range(0, nx):
        #     data_to_send[i] = analysis_list[i].Get_LL_G()
        # gathered_G = data_to_send


        # # Send the minimum values
        # ny_slices = len(time_slices)
        # data_to_send = np.zeros((nx, ny_slices), dtype='d')
        # for i in range(0, nx):
        #     slices = analysis[i].Get_time_slices()
        #     for j in range (0, ny_slices):
        #         data_to_send[i][j] = slices[j].Min
        # gathered_data_slices = self._receive_2D_data(nx, ny_slices, data_to_send)
        #self.comm.Barrier()

        di = pd.DataFrame(np.array(parameter_list), columns=full_parameters.columns)
        di['LL'] = ll_list

        ds = di.to_xarray()

        ds.to_netcdf(f"{self.config.output_path}ParameterRanks{self.rank}.nc")



    def _receive_2D_data(self, nx, ny, data_to_send):

        count_transfer = self.n_array_per_process * nx
        sendbuf = data_to_send
        recvbuf = np.zeros((sum(self.n_array_per_process), ny), dtype='d')

        if self.is_root:
            displ = np.copy(self.displ)
            displ *= ny
        else:
            displ =  self.displ

        print(f"Rank {self.rank} count {self.n_array_per_process}")
        print(f"Rank {self.rank} disp {self.displ}")

        self.comm.Gatherv(sendbuf, [recvbuf, count_transfer,  displ, MPI.DOUBLE], root=0)
        # if self.is_root == 0:
        #     print('After Gatherv, process 0 has data:', recvbuf)
        return recvbuf;

