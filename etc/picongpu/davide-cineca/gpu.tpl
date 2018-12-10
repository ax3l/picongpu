#!/usr/bin/env bash
# Copyright 2013-2018 Axel Huebl, Richard Pausch, Rene Widera
#
# This file is part of PIConGPU.
#
# PIConGPU is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# PIConGPU is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with PIConGPU.
# If not, see <http://www.gnu.org/licenses/>.
#


# PIConGPU batch script for davide' SLURM batch system

# A Davide node contains 2 sockets with a Power8 processors with 8 cores and 8x hyperthreading.
# Each node has accesss to 4 P100 gpus. PIConGPu will use 2 MPI tasks per socket with each 32 OpenMP threads.
# The batch system limits the allocation of cores per tasks to 16.
# To allow the usage of all cores we allocate twice as much mpi tasks as required for PIConGPU.
# This overallocation is fixed later on with `srun` where the number of tasks per socket
# is limited to 2 and the correct number of tasks are spawned.

#SBATCH --account=!TBG_nameProject
#SBATCH --partition=!TBG_queue
#SBATCH --time=!TBG_wallTime
# Sets batch job's name
#SBATCH --job-name=!TBG_jobName
#SBATCH --nodes=!TBG_nodes
#SBATCH --ntasks=!TBG_allocNTasks
# 16 is the maximum allowed by the abtch system
#SBATCH --cpus-per-task=16
#SBATCH --mem=!TBG_memPerNode
#SBATCH --gres=gpu:!TBG_gpusPerNode
#SBATCH --gres-flags=enforce-binding
#SBATCH --mail-type=!TBG_mailSettings
#SBATCH --mail-user=!TBG_mailAddress
#SBATCH --workdir=!TBG_dstPath
#SBATCH --workdir=!TBG_dstPath

#SBATCH -o stdout
#SBATCH -e stderr


## calculations will be performed by tbg ##
.TBG_queue="dvd_usr_prod"

# settings that can be controlled by environment variables before submit
.TBG_mailSettings=${MY_MAILNOTIFY:-"NONE"}
.TBG_mailAddress=${MY_MAIL:-"someone@example.com"}
.TBG_author=${MY_NAME:+--author \"${MY_NAME}\"}
.TBG_nameProject=${proj:-""}
.TBG_profile=${PIC_PROFILE:-"~/picongpu.profile"}

# number of available/hosted GPUs per node in the system
.TBG_numHostedGPUPerNode=4

# required GPUs per node for the current job
.TBG_gpusPerNode=`if [ $TBG_tasks -gt $TBG_numHostedGPUPerNode ] ; then echo $TBG_numHostedGPUPerNode; else echo $TBG_tasks; fi`
.TBG_allocNTasks=$(( $TBG_tasks * 2 ))

# host memory per gpu
.TBG_memPerGPU="$((254000 / $TBG_numHostedGPUPerNode))"
# host memory per node
.TBG_memPerNode="$((TBG_memPerGPU * TBG_gpusPerNode))"

# use ceil to caculate nodes
.TBG_nodes="$((( TBG_tasks + TBG_gpusPerNode - 1 ) / TBG_gpusPerNode))"

## end calculations ##

echo 'Running program...'

cd !TBG_dstPath

export MODULES_NO_OUTPUT=1
source !TBG_profile
if [ $? -ne 0 ] ; then
  echo "Error: PIConGPU environment profile under \"!TBG_profile\" not found!"
  exit 1
fi
unset MODULES_NO_OUTPUT

#set user rights to u=rwx;g=r-x;o=---
umask 0027

mkdir simOutput 2> /dev/null
cd simOutput

# test if cuda_memtest binary is available and we have the node exclusive
if [ -f !TBG_dstPath/input/bin/cuda_memtest ] && [ !TBG_numHostedGPUPerNode -eq !TBG_gpusPerNode ] ; then
  # Run CUDA memtest to check GPU's health
  srun --cpu-bind=sockets !TBG_dstPath/input/bin/cuda_memtest.sh
else
  echo "no binary 'cuda_memtest' available or compute node is not exclusively allocated, skip GPU memory test" >&2
fi

export OMP_NUM_THREADS=32

if [ $? -eq 0 ] ; then
  # Run PIConGPU correct number of tasks
  srun --ntasks=!TBG_tasks -cpu-bind=sockets --ntasks-per-socket=2 !TBG_dstPath/input/bin/picongpu !TBG_author !TBG_programParams | tee output
fi
