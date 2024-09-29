import unittest
import subprocess
import os
import xarray
from src.contrib.auxil.messaging import print_sucess, print_failure
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_Multi_Mpi(unittest.TestCase):

    def test_mpi(self):
        print("Testing MPI...", end='')
        command = "mpiexec --help"
        process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        if error == None:
            found_mpi = True
            print_sucess("OK!")
        else:
            found_mpi = False
            print_failure("Failed!")

        self.assertTrue(found_mpi)


    def test_mpi_hainich_mpi_simulation(self):
        print("Testing if simulation be executed via MPI", end='')
        command = "mpiexec -n 2 python mpi_multi_local.py"
        process = subprocess.Popen(command.split(), stdout=subprocess.PIPE, cwd = THIS_DIR)
        output, error = process.communicate()

        if error == None:
            found_mpi = True
            print_sucess("OK!")
        else:
            found_mpi = False
            print_failure("Failed!")

        self.assertTrue(found_mpi)

    def test_check_mpi_output(self):
        print("Testing if output is correct...", end='')

        output_file = os.path.join(THIS_DIR, 'test', 'output', 'Multi_Out.nc')

        ds = xarray.open_dataset(output_file)


        REFERENCE_J_RMSE = [0.002554028773026848, 0.000693889552292396]
        REFERENCE_PSI_STEM_RMSE = [0.7289339483826249, 1.9520272480037195]
        EPS = 8


