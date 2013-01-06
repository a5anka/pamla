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
# packages["facesim"]="apps" # No test data
packages["ferret"]="apps"
packages["fluidanimate"]="apps"
packages["freqmine"]="apps"
# packages["raytrace"]="apps" # Not building access(x,y) method missing
packages["streamcluster"]="kernels"
packages["swaptions"]="apps"
packages["vips"]="apps"
# packages["x264"]="apps" Gives segmentation faults

inputs=(simsmall simmedium simlarge) # add native
THREAD_LIST=(1 4 8 12)
perfEvents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"

for program in ${!packages[@]}; do
    bmdir="${PARSEC_DIR}/${benchdir}/${packages[$program]}/${program}"
    bminputdir="${bmdir}/inputs"
    bmparsecdir="${bmdir}/parsec"

    mkdir -p $bmrundir
    pushd "${bmrundir}" > /dev/null

    for input in ${inputs[@]}; do
        bminput="${bminputdir}/input_${input}.tar"
        if [ -f $bminput ]; then
            ${UNTAR} ${bminput} > /dev/null
        fi

        for threads in ${THREAD_LIST[@]}; do
            if [ $threads -eq 12 ] && [ $program == "fluidanimate" ]; then
                continue
            fi

            NTHREADS=$threads

            source ${bmparsecdir}/${input}.runconf
            bmexec_suffix=${run_exec}
            bminstdir="${bmdir}/inst/${PARSECPLAT}"
            bmexec="${bminstdir}/${bmexec_suffix}"
            bmexec_args=${run_args}

            echo "#PROGRAM=$program:INPUT=$input:NTHREADS=$NTHREADS"
            time $bmexec $bmexec_args > /dev/null
            echo
        done
    done

    popd > /dev/null
    rm -r $bmrundir

done
