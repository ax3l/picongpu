from mpi4py import MPI
import sys

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

print(rank)

from pypicongpu import *
sim=SimStarter()

sim.parseConfigs(sys.argv)
sim.load()
sim.start()
sim.unload()
