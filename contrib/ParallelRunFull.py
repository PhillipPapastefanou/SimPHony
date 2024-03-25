import numpy as np
import pandas as pd
import mpi4py.MPI as MPI
import xarray as xr
from time import perf_counter

from hydro_standalone import Simulation_Multi_Hainich
from hydro_standalone import DateTime
from contrib.config import Config

class ParallelSetupFull:
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
        df_i = pd.read_csv(self.config.parameter_input_file_list)
        n = df_i.shape[0]

        self.sendbuf = np.linspace(0, n - 1, num=n).astype('i')

        ave, res = divmod(self.sendbuf.size, self.size)
        self.count = [ave + 1 if p < res else ave for p in range(self.size)]
        self.count = np.array(self.count)

        # displacement: the starting index of each sub-task
        self.displ = [sum(self.count[:p]) for p in range(self.size)]
        self.displ = np.array(self.displ)

    def _initialise_counts(self):
        self.sendbuf = None
        # initialize count on worker processes
        self.count = np.zeros(self.size, dtype=int)
        self.displ = None


    def send_parameter_indexes(self):

        if self.is_root:
            print("Broadcasting parameter indices...", end = '')
            t1 = perf_counter()

        # broadcast The number of parameter files each process will get
        self.comm.Bcast(self.count, root=0)

        # Initialize the memory according to that file size
        self.recvbuf = np.zeros(self.count[self.rank], dtype='i')

        # Send the indexes ot each process
        self.comm.Scatterv([self.sendbuf, self.count, self.displ, MPI.INTEGER], self.recvbuf, root=0)
        # Print the chunk that was received by this process
        # print("Process {} received chunk {}".format(self.rank, recvbuf))

        self.comm.Barrier()

        if self.is_root:
            t2 = perf_counter()
            print(f"Done ({np.round(t2-t1, 1)}) sec.")


    def start_simulations(self):

        if self.is_root:
            print("Starting simulations...", end = '')
            t1 = perf_counter()

        self.sim = Simulation_Multi_Hainich()
        self.sim.Init_Full_Parameter_Setups(self.config.parameter_input_file_list, self.recvbuf)
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


    def receive_analysis_data(self):
        analysis = self.sim.Get_analysis_list()
        # Get first time_slice data
        #time_slices = analysis[0].Get_time_slices()
        # Get first RMSE datapoint
        rmse_J = analysis[0].Get_Rmse_J()
        rmse_G = analysis[0].Get_Rmse_G()


        if self.is_root:
            print("Sending data to root...", end = '')
            t1 = perf_counter()

        # Send the RMSE datasets
        nx = self.recvbuf.shape[0]
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
                 "RMSE_G": (("run_id"), np.squeeze(gathered_data_rmse_G))
                    #,"Minimum": (("run_id", "slices_id"), gathered_data_slices)
                 },
                coords={
                    "run_id": np.arange(0, sum(self.count)),
                    #"rmse": np.arange(0, ny_rmse),
                    #"slices_id": np.arange(0, ny_slices),
                },
            )
            ds.to_netcdf('Sens_Output.nc', encoding={"RMSE_J": {"dtype": "single"},
                                                     "RMSE_G": {"dtype": "single"}})
            t2 = perf_counter()
            print(f"Done ({np.round(t2 - t1, 1)}) sec.")


    def _receive_2D_data(self, nx, ny, data_to_send):

        count_transfer = self.count * nx
        sendbuf = data_to_send
        recvbuf = np.zeros((sum(self.count), ny), dtype='d')

        if self.is_root:
            displ = np.copy(self.displ)
            displ *= ny
        else:
            displ =  self.displ

        print(f"Rank {self.rank} count {self.count}")
        print(f"Rank {self.rank} disp {self.displ}")

        self.comm.Gatherv(sendbuf, [recvbuf, count_transfer,  displ, MPI.DOUBLE], root=0)
        # if self.is_root == 0:
        #     print('After Gatherv, process 0 has data:', recvbuf)
        return recvbuf;

