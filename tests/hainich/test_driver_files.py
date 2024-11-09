import unittest
import os
from src.contrib.auxil.messaging import print_sucess, print_failure
from src.contrib.auxil.files import get_forcing_filepath_hainich
from src.contrib.auxil.files import get_sapflow_filepath_hainich
from src.contrib.auxil.files import get_psi_stem_filepath_hainich
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_Driver_Files(unittest.TestCase):

    def test_forcing_file(self):

        print("Checking if forcing file exists...", end='')
        forcing_file = get_forcing_filepath_hainich()
        exists = os.path.exists(forcing_file)
        if exists:
            print_sucess("OK!")
        else:
            print_failure("Not found!")
        self.assertTrue(exists)

    def test_sap_file(self):
        print("Checking if sap file exists...", end='')
        sap_file = get_sapflow_filepath_hainich()
        exists = os.path.exists(sap_file)
        if exists:
            print_sucess("OK!")
        else:
            print_failure("Not found!")
        self.assertTrue(exists)

    def test_psi_stem_file(self):
        print("Checking if psi_stem file exists...", end='')
        psi_stem_file = get_psi_stem_filepath_hainich()
        exists = os.path.exists(psi_stem_file)
        if exists:
            print_sucess("OK!")
        else:
            print_failure("Not found!")
        self.assertTrue(exists)
