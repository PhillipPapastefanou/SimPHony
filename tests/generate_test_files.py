import sys
import os
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, os.pardir))
from src.contrib.param_generation.example_generator import create_example_hainich_parameter_list
from src.contrib.param_generation.example_generator import create_example_swiss_cc_parameter_list

def main():
    os.makedirs(os.path.join("hainich","test","input"), exist_ok=True)
    os.makedirs(os.path.join("swiss_cc","test","input"), exist_ok=True)

    for i in range(1,3):
        create_example_hainich_parameter_list(i, os.path.join("hainich","test","input",f"parameter_example_{i}.csv"))
        create_example_swiss_cc_parameter_list(i, os.path.join("swiss_cc","test","input",f"parameter_example_{i}.csv"))

if __name__ == "__main__":
    main()