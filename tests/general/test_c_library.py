import unittest
import os
import glob
import sys
import subprocess
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir, os.pardir))
from src.contrib.auxil.files import get_SimPHony_test_build_path

class Test_C_Library(unittest.TestCase):
    def test_check_SimPHony_tests_exists(self):
        found_lib, lib_path, lib_folder = get_SimPHony_test_build_path()
        self.assertTrue(found_lib, f"Could not find SimPHony library build.")
        if found_lib:
            os.makedirs(os.path.join(THIS_DIR,'test'), exist_ok=True)
            with open(os.path.join(THIS_DIR, "test","SimPHony_tests_path.txt"), "w") as file:
                file.write(lib_path)

if __name__ == "__main__":
    Test_C_Library().test_check_SimPHony_tests_exists()