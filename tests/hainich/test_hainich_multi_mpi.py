import unittest
import subprocess
import os
import xarray
import sys
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
        print("Testing if the netcdf output is correct...", end='')

        output_file = os.path.join(THIS_DIR, 'test', 'output', 'Multi_Output0.nc')

        ds = xarray.open_dataset(output_file)

        REFERENCE_J_RMSE = [0.002554028773026848, 0.000693889552292396]
        REFERENCE_PSI_STEM_RMSE = [0.7289339483826249, 1.9520272480037195]
        EPS = 7

        # Compare to setup
        for i in range(ds.sizes['run_id']):
            self.assertAlmostEqual(ds['RMSE_psi_stem'][i].values, REFERENCE_PSI_STEM_RMSE[i], places=EPS)
            self.assertAlmostEqual(ds['RMSE_J'][i].values, REFERENCE_J_RMSE[i], places=EPS)
        print("Done!")


