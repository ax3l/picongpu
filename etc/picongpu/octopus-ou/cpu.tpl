#!/usr/bin/env bash
# Copyright 2013-2018 Axel Huebl, Anton Helm, Rene Widera
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


# PIConGPU batch script for NEC NQSII batch system

#PBS -q !TBG_queue
# PBS -A <acctcode>
# PBS -T mpisx
#PBS -l elapstim_req=!TBG_wallTime
# Sets batch job's name
#PBS -b !TBG_nodes
#PBS -l cpunum_job=!TBG_coresPerNode
#PBS -N !TBG_jobName
# send me mails on job (b)egin, (e)nd, (a)bortion or (n)o mail
#PBS -m !TBG_mailSettings -M !TBG_mailAddress
#PBS -d !TBG_dstPath

#PBS -o stdout
#PBS -e stderr


## calculation are done by tbg ##
.TBG_queue="OCTOPUS"

# settings that can be controlled by environment variables before submit
.TBG_mailSettings=${MY_MAILNOTIFY:-"n"}
.TBG_mailAddress=${MY_MAIL:-"someone@example.com"}
.TBG_author=${MY_NAME:+--author \"${MY_NAME}\"}
.TBG_profile=${PIC_PROFILE:-"~/picongpu.profile"}

# Two 12-core skylake CPUs per node if we need more than 2 else same count as TBG_tasks
.TBG_cpusPerNode=`if [ $TBG_tasks -gt 24 ] ; then echo 24; else echo $TBG_tasks; fi`

# 2x12 Xeon cores per node
# number of cores per parallel node
.TBG_coresPerNode="$(( TBG_cpusPerNode ))"

# use ceil to caculate nodes
.TBG_nodes="$(( ( TBG_tasks + TBG_cpusPerNode -1 ) / TBG_cpusPerNode))"
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

#wait that all nodes see ouput folder
sleep 1

if [ $? -eq 0 ] ; then
  mpiexec -ppn !TBG_cpusPerNode -n !TBG_tasks !TBG_dstPath/input/bin/picongpu !TBG_author !TBG_programParams | tee output
fi

mpiexec -ppn !TBG_cpusPerNode -n !TBG_tasks /usr/bin/env bash -c "killall -9 picongpu 2>/dev/null || true"
