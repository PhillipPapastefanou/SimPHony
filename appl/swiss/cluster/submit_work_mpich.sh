#!/bin/bash
#SBATCH -J QUINCY_QPY
#SBATCH --error=%j.err
#SBATCH --output=%j.log
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --export=ALL
#SBATCH --time=200:00:00
#SBATCH --nodes=10
#SBATCH --ntasks=640
#SBATCH --partition='work'
#SBATCH --mem='300G'

module purge
#module -q load gnu12 R/4.3.2
#module -q load openmpi4 netcdf
#ml all/Miniconda3

#ml intel/2023.0.0  impi/2021.6.0
#ml netcdf/4.9.0
ml all/Miniconda3

ml intel/2023.0.0  mpich/3.4.3-ofi
#ml gnu12/12.2.0 mpich/3.4.3-ofi
ml netcdf/4.9.0

source ~/.bash_profile
echo "QUINCY path: $QUINCY"

source /User/homes/ppapastefanou/miniconda3/etc/profile.d/conda.sh
#conda activate /Net/Groups/BSI/work_scratch/ppapastefanou/envs/phs
#conda activate /Net/Groups/BSI/work_scratch/ppapastefanou/envs/QPy_gnu
#conda activate /Net/Groups/BSI/work_scratch/ppapastefanou/envs/QPy_gnu_mpich
conda activate /Net/Groups/BSI/work_scratch/ppapastefanou/envs/SimPHony_intel_mpich
which python

export FI_PROVIDER=tcp

mpirun -n 640 /Net/Groups/BSI/work_scratch/ppapastefanou/envs/SimPHony_intel_mpich/bin/python -u swiss_cc_mpi_cluster_EX_CON.py