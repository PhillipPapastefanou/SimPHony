import unittest
import os
from src.contrib.auxil.messaging import print_sucess, print_failure
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Test_Driver_Files(unittest.TestCase):

    def test_forcing_file(self):

        print("Cecking if forcing file exists...", end='')
        forcing_file = os.path.join(THIS_DIR, os.pardir, os.pardir,
                                    'data/hainich/input/Meteo_Hainich_dT30min_forcing_PHS.csv')
        exists = os.path.exists(forcing_file)


        if exists:
            print_sucess("OK!")
        else:
            print_failure("Not found!")
        self.assertTrue(exists)

    def test_sap_file(self):
        print("Cecking if sap file exists...", end='')
        sap_file = os.path.join(THIS_DIR, os.pardir, os.pardir,
                                    'data/hainich/eval/SAP_Hainich_Fagus-mean_dT30min_prog.csv')
        exists = os.path.exists(sap_file)
        if exists:
            print_sucess("OK!")
        else:
            print_failure("Not found!")
        self.assertTrue(exists)


    def test_psi_stem_file(self):
        print("Cecking if psi_stem file exists...", end='')
        psi_stem_file = os.path.join(THIS_DIR, os.pardir, os.pardir,
                                    'data/hainich/eval/stem_water_pot.csv')
        exists = os.path.exists(psi_stem_file)

        if exists:
            print_sucess("OK!")
        else:
            print_failure("Not found!")
        self.assertTrue(exists)
