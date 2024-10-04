import unittest
from tests.general.test_check_libs import Test_General_Setup
from tests.general.tes import Test_General_Setup
from tests.hainich.test_driver_files import Test_Driver_Files
from tests.hainich.test_hainich_direct import Test_Hainich_Direct
from tests.hainich.test_hainich_from_file import Test_Hainich_From_File
from tests.hainich.test_hainich_from_file_multi import Test_Hainich_From_File_Multi
from tests.hainich.test_hainich_multi_mpi import Test_Multi_Mpi

if __name__ == '__main__':
    unittest.main()
