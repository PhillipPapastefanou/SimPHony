#!/bin/bash
#SBATCH -J SimPHony
#SBATCH --error=%j.err
#SBATCH --output=%j.log
#SBATCH -D ./
#SBATCH --get-user-env
#SBATCH --mail-type=ALL
#SBATCH --mail-user=papa@tum.de
#SBATCH --export=NONE
#SBATCH --time=100:00:00
#SBATCH --nodes=3
#SBATCH --ntasks=384
#SBATCH --partition='big'
#SBATCH --mem='1600G'

module purge
ml intel/2023.0.0 impi/2021.6.0
ml netcdf/4.9.0
ml all/Miniconda3

source /User/homes/ppapastefanou/miniconda3/etc/profile.d/conda.sh
#conda info --envs
conda activate /Net/Groups/BSI/work_scratch/ppapastefanou/envs/SimPHony_intel_mpich
which python

export FI_PROVIDER=tcp

mpiexec -n 384 python3 -u swiss_cc_mpi_cluster_EX_CON.py
