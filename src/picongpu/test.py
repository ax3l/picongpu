from mpi4py import MPI
import sys
import ctypes

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

print(rank)

import pypicongpu
sim=pypicongpu.SimStarter()

sim.parseConfigs(sys.argv)
sim.load()
