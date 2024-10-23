import sys
import os
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir))

import unittest
from tests.general.test_check_libs import Test_General_Setup
from tests.general.test_c_library import Test_C_Library

from tests.hainich.test_driver_files import Test_Driver_Files
from tests.hainich.test_hainich_direct import Test_Hainich_Direct
from tests.hainich.test_hainich_from_file import Test_Hainich_From_File
from tests.hainich.test_hainich_from_file_multi import Test_Hainich_From_File_Multi
from tests.hainich.test_hainich_multi_mpi import Test_Multi_Mpi

from tests.swiss_cc.test_swiss_cc_direct import Test_Swiss_Direct
from tests.swiss_cc.test_swiss_cc_from_file import Test_Swiss_From_File

if __name__ == '__main__':
    unittest.main()
