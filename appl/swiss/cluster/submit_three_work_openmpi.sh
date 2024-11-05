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
#SBATCH --nodes=3
#SBATCH --ntasks=192
#SBATCH --partition='work'
#SBATCH --mem='300G'
module purge
ml intel/2023.0.0  openmpi4/4.1.4
ml netcdf/4.9.0
ml all/Miniconda3

source /User/homes/ppapastefanou/miniconda3/etc/profile.d/conda.sh
#conda info --envs
conda activate /Net/Groups/BSI/work_scratch/ppapastefanou/envs/SimPHony_intel_openmpi

which python
which mpiexec
which orted

export FI_PROVIDER=tcp

#mpiexec -n 00 -mca btl openib,self python3 swiss_cc_mpi_cluster_LHS.py
#mpiexec -n 100 -mca btl self,tcp python3 swiss_cc_mpi_cluster_LHS.py
mpiexec -n 192 /Net/Groups/BSI/work_scratch/ppapastefanou/envs/SimPHony_intel_openmpi/bin/python swiss_cc_mpi_cluster_LHS.py
