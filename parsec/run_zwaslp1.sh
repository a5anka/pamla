#!/usr/bin/env bash

PARSEC_DIR=/opt/parsec-3.0
PARSECPLAT="powerpc-linux.gcc"
UNTAR="tar -xvf"
PERF=/usr/local/bin/perf
benchdir="pkgs"
bmrundir=/tmp/parsec-run

: <<END
declare -A packages
packages["blackscholes"]="apps"
packages["bodytrack"]="apps"
packages["canneal"]="kernels"
packages["dedup"]="kernels"
packages["facesim"]="apps" # No test data
packages["ferret"]="apps"
packages["fluidanimate"]="apps"
packages["freqmine"]="apps"
#packages["raytrace"]="apps" # Not building access(x,y) method missing
packages["streamcluster"]="kernels"
packages["swaptions"]="apps"
packages["vips"]="apps"
packages["x264"]="apps" # Gives segmentation faults
END

programs="blackscholes bodytrack canneal dedup facesim ferret fluidanimate freqmine streamcluster swaptions vips x264"

inputs=(simsmall simmedium simlarge) # add native

THREAD_LIST=(1 2 4 8 16 32)

perfEvents="r30083,r4d048,r30064,r200fe,rd0a0,rd8ac,rd0a1,rd0a2,r4003e,rd0ac,r26880,r3c05c,r26182,r400f0,r1c048,r10081,r10083,r26080,r200f6,rd0aa,r1c04e,r4f080,rd0a8,r300f6,r2d05c,r20016,r1d048,rd8a8,r16080,r2,r3d05c,r1d04e"

if [ -d $bmrundir ]; then
    rm -r $bmrundir
fi

for program in $programs 
do
if [ $program = "canneal" ] || [ $program = "dedup" ] || [ $program = "streamcluster" ]
then
bmdir="${PARSEC_DIR}/${benchdir}/kernel/${program}"
else
bmdir="${PARSEC_DIR}/${benchdir}/apps/${program}"
fi

    #bmdir="${PARSEC_DIR}/${benchdir}/${packages[$program]}/${program}"
    bminputdir="${bmdir}/inputs"
    bmparsecdir="${bmdir}/parsec"

    for input in ${inputs[@]}; do
        mkdir -p $bmrundir
        pushd "${bmrundir}" > /dev/null

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
	    sudo $PERF stat -r 3 -e $perfEvents $bmexec $bmexec_args > /dev/null
            echo
        done

        popd > /dev/null
        rm -r $bmrundir

    done
done
