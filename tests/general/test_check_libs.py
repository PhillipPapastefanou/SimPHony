import unittest
import os
import glob
import sys
import importlib.util
from src.contrib.auxil.files import get_SimPHony_build_path


THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_General_Setup(unittest.TestCase):

    def test_check_packages(self):
        for var in ['colorama', 'numpy', 'scipy', 'pandas' , 'mpi4py', 'subprocess', 'xarray']:
            print("Checking packages for " + var + "...", end ='')
            if importlib.util.find_spec(var) is None:
                found = False
                print("Not found!")
            else:
                from src.contrib.auxil.messaging import print_sucess
                found = True
                print_sucess("Found!")
            self.assertTrue(found, f"Could not find package {var}")

    def test_check_SimPHony_lib(self):
        from src.contrib.auxil.messaging import print_sucess, print_failure
        print("Checking for SimPHony build...", end ='')
        found_lib, lib_path, lib_folder = get_SimPHony_build_path()

        if found_lib:
            print_sucess("Found!")
            print(f"Lib path: P{lib_path}")
        else:
            print_failure("Not found!")

        self.assertTrue(found_lib, f"Could not find SimPHony library build.")
