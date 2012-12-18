#!/usr/bin/env bash

PARSEC_DIR=/home/asanka/parsec-3.0
PARSECPLAT="i686-linux.gcc"
UNTAR="tar -xvf"
PERF=perf
benchdir="pkgs"
bmrundir=/tmp/parsec-run

declare -A packages
packages["blackscholes"]="apps"
packages["bodytrack"]="apps"
packages["canneal"]="kernels"
packages["dedup"]="kernels"
packages["facesim"]="apps"
packages["ferret"]="apps"
packages["fluidanimate"]="apps"
packages["freqmine"]="apps"
packages["raytrace"]="apps"
packages["streamcluster"]="kernels"
packages["swaptions"]="apps"
packages["vips"]="apps"
packages["x264"]="apps"

inputs=(test simdev simsmall simmedium simlarge) # add native 

# TODO iterate over set of thread numbers
NTHREADS=1

perfEvents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"

for program in ${!packages[@]}; do
    bmdir="${PARSEC_DIR}/${benchdir}/${packages[$program]}/${program}"
    bminputdir="${bmdir}/inputs"
    bmparsecdir="${bmdir}/parsec"

    mkdir -p $bmrundir
    pushd "${bmrundir}" > /dev/null

    for input in ${inputs[@]}; do
        echo "# Extracting $input for $program"
        bminput="${bminputdir}/input_${input}.tar"
        if [ -f $bminput ]; then
            ${UNTAR} ${bminput} > /dev/null
        fi

        source ${bmparsecdir}/${input}.runconf
        bmexec_suffix=${run_exec}
        bminstdir="${bmdir}/inst/${PARSECPLAT}"
        bmexec="${bminstdir}/${bmexec_suffix}"
        bmexec_args=${run_args}
        $PERF stat -x : -r 3 -e $perfEvents $bmexec $bmexec_args > /dev/null

        echo
    done
    
    popd > /dev/null
    rm -r $bmrundir

done
