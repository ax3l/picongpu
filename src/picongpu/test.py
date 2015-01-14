from mpi4py import MPI
import sys
import ctypes

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

print(rank)

import pypicongpu
sim=pypicongpu.SimStarter()

#arr = (ctypes.c_char_p * (len(sys.argv) + 1))()
#arr[:-1] = sys.argv
#arr[ len(sys.argv) ] = None

#a=ctypes.c_char(arr[0][0])
#aptr = ctypes.pointer(a)

#print arr, a
#print aptr

string_length = len(sys.argv)

select_type = (ctypes.c_char_p * string_length)
select = select_type()

for key, item in enumerate(sys.argv):
    select[key] = item

#library.passPointerArray.argtypes = [c_int, select_type]
#library.passPointerArray(string_length, select)

sim.parseConfigs(len(sys.argv), select)
sim.load()
