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
Get CMake. Get a cpp compiler and an mpi library. It should at least support the 2017 standard. We recommend clang or intel compiles as the model runs up to 5x slower when compiled with GNU.
Create a Python repository that contains the following packages:
* numpy
* pandas
* xarray netcdf
* mpi4py
* scipy
* ...
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

## Tests
Before running the examples scripts one shoud make sure that SimPHony has been build sucessfully. Therefore unittests both exist for cpp library and the python scripts. 
The cpp unittests can either be run by some IDE (integrated development environment, such as Pycharm or VS-code) or directly by executing the binary. The Python unittests can also be run via IDE or from the terminal in the main directory:
``
python -m unittest tests/main.py
``
## Examples
Examples can be found the example/hainich folder. 


