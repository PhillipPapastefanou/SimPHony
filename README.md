# The SimPHony model 

The **Sim**ple **P**lant **H**ydraulics **on**l**y** model is standalone modelling tool that describes the water flow throught a plant via roots, stem and leaves 
based on the fundamental laws of water transport.

### Name origin
The name simphony is an acronym for symphony, which generally refers to a large-scale orchestral composition
with a complex structure, typically consisting of four movements. Despite its simplicity, the SimPHony model can also exhibit complex behavior and its
major parameters need to (harmonically) play together to mimic realistic plant water flows and pools. Furthermore, the SimPHony model development was exclusively accompanied by classical symphonies by renowned composers such as Bach and Beethoven.
To maintain the model's integrity and ensure its continued evolution, this musical tradition must be upheld in future development endeavors.

# Theory
Fundamental equations (to be explained):
```math
    \kappa_\mathrm{Leaf}\cdot \frac{d\Psi_{\mathrm{Leaf}}}{dt} - J + T = 0
```

```math
       \kappa_{\mathrm{Stem}} \cdot \frac{d\Psi_{\mathrm{Stem_G}}}{dt} + J - G +
T_\mathrm{bark} = 0
```
# Application
## Prerequisites 
Get CMake. Get a cpp compiler and a mpi library. It should at least support the 2017 standard. We recommend clang or intel compiles as the model runs up to 5x slower when compiled with GNU.
Create a Python environment that contains the following packages:
* numpy
* pandas
* xarray netcdf
* mpi4py
* scipy
* ...
Note: When setting up the python environment on an HPC: Make sure to link your mpi4py installation to the existing MPI setup
## Setup
Clone this repository and create a build folder in the main direcorty. Navigate to that build directory in the terminal and run:
``
cmake ../
``.
Once that build system is finished successfully, run 
``
make
``.
This should (amongst other) create two files: A binary/executable called SimPHony_tests and a python library ending with `*.so`.
### MPI-BGC cluster setup
MPI BGC cluster supports intel and gnu cpp compiler and intel and openmpi MPI libraries. So far only intel cpp libraries have been used as these make the model ~5x faster compared to GNU.
Futhermore, with intel cpp compiler only intelmpi has been successfully tested.
#### Intel setup
To build the SimPHony library we first need to have our python environment setup.
Therefor, we load the intel cpp and mpi libary versions:
``
ml intel/2023.0.0  impi/2021.6.0
ml netcdf/4.9.0
ml all/Miniconda3
``
We need to figure out where the mpi compiler is located.
``
which mpicc
``
which for example yields `/opt/intel/oneapi/mpi/2021.6.0/bin/mpicc`
Then we create a new pyhon environment and install the libraries:

```
conda create --prefix /Net/Groups/BSI/work_scratch/<user>/envs/SimPHony_intel_oneapi
conda activate /Net/Groups/BSI/work_scratch/<user>/envs/SimPHony_intel_oneapi

conda config --add pkgs_dirs /Net/Groups/BSI/work_scratch/<user>/pkgs/

conda install -y matplotlib
conda install -y conda-forge::scipy
 env MPICC=/opt/intel/oneapi/mpi/2021.6.0/bin/mpicc python -m pip install --no-cache-dir  mpi4py
conda install -y -c conda-forge xarray netCDF4
conda install -y numpy
conda install -y pandas
conda install -y conda-forge::pybind11
```

To build SimPHony we activate the before created environment. Then we have to make sure to export the correct cpp libraries
``
export CC=/opt/intel/oneapi/compiler/2023.0.0/linux/bin/intel64/icc
export CXX=/opt/intel/oneapi/compiler/2023.0.0/linux/bin/intel64/icpc
``
We can also get these with the `which` command.
Finally, we build SimPHony using cmake and as described above.


## Tests
Before running the example scripts one should make sure that SimPHony has been built sucessfully. Therefore, unittests both exist for cpp library and the python scripts. 
The cpp unittests can either be run by some IDE (integrated development environment, such as Pycharm or VS-code) or directly by executing the binary. The Python unittests can also be run via IDE or from the terminal in the main directory:
``
python -m unittest tests/main.py
``
## Usage

For now, each simulated location (for now Hainich and Swiss_cc) that SimPHony can be applied requires specific setups and analysis scripts. Therefore testing/example and application scripts are tailored to the respective location. \\
SimPHony has a **single** and **multi** application variant:
* The **single** variant returns all the model states and derived variables per timestep. It also contains analysis output that condenses the model output, by for example calculating the RMSE to observed sapflow. Its intended use is to plot few (~ 100) time series of model outputs. 
* The **multi** variant only returns the the analysis data. It does NOT return the output of the model to save memory. Its intended use is sensitivity analysis and therefore run tens of thousands of model simulations.  

### Examples
Examples can be found the example/hainich folder. 


