
echo "Performing all tests related to SimPHony..."
echo "----------------------------------------------------"
python general/test_c_library.py
echo "----------------------------------------------------"
python generate_test_files.py

test_bin=$(head -n 1 general/test/SimPHony_tests_path.txt)

echo "Testing the C library... "
"$test_bin"
echo "Successfully performed all test of the SimPHony binary"
echo "----------------------------------------------------"
echo "Running python libary tests..."
python -m unittest run_tests.py
