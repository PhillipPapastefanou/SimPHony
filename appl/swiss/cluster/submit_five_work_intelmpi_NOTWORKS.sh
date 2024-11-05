#!/bin/bash
#SBATCH -J GUESS_NCPS
#SBATCH --error=%j.err
#SBATCH --output=%j.log
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --mail-type=ALL
#SBATCH --mail-user=papa@tum.de
#SBATCH --export=ALL
#SBATCH --time=24:00:00
#SBATCH --nodes=5
#SBATCH --ntasks=320
#SBATCH --partition='work'
#SBATCH --mem='300G'
module purge
ml intel/2023.0.0  impi/2021.6.0
ml netcdf/4.9.0
ml all/Miniconda3

source /User/homes/ppapastefanou/miniconda3/etc/profile.d/conda.sh
#conda info --envs
conda activate /Net/Groups/BSI/work_scratch/ppapastefanou/envs/SimPHony_intel_oneapi

which python
which mpiexec
which orted

export FI_PROVIDER=tcp

mpiexec -n 320 /Net/Groups/BSI/work_scratch/ppapastefanou/envs/SimPHony_intel_oneapi/bin/python -u swiss_cc_mpi_cluster_LHS.py

