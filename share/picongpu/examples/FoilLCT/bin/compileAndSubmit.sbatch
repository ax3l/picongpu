#!/usr/bin/env bash
#
#SBATCH --partition=gpu
#SBATCH --time=16:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=24
#SBATCH --mail-type=ALL
#SBATCH --mail-user=a.huebl@hzdr.de
#SBATCH -o compileAndSubmit.stdout
#SBATCH -e compileAndSubmit.stderr

. $HOME/gpu_picongpu.profile
export TBG_SUBMIT="/usr/bin/sbatch"

inputDir="/home/huebl/picInputs/foilLCT_fm"
runsDir="/bigdata/hplsim/development/huebl/foilLCT_fm"

# prepare analysis dirs
mkdir -p ${runsDir}/{energyEvolution,finalEnergy}

# build and submit
cd $inputDir
rm -rf .build/

for t in {0..5}
do
    pic-build -t $t
    tbg -s -t -c etc/picongpu/8.cfg \
        $runsDir/${t}00
    tbg -s -t -c etc/picongpu/8.cfg \
        $runsDir/${t}01
done

# copy representative analysis scripts
cp -a bin/*.py $runsDir/
