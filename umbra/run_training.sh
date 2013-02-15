#!/bin/sh
#
#	run.sh
#
#	As “perf” sends output to “stderr”, collect data onto a file “myfile.dat” as follows
#	shell%>./run-read.sh  &> myfile.dat
#	Try on snb (t420):
#	sudo perf stat -e r00c0 ./psum-2-pad 2 28
#	Try on nhm (commit5):
#	sudo perf stat -e r00c0 ./psum 2 28
#
#   DynamoRIO library has to be installed and the umbra framework has to be built on the machine.
#   Set the path to the two on the script.
#
pmat=pmatrixmult
pdot=pdotproduct
psums=psumscalar
psumv=psumvector
count=counting
pad=padding
false1=false1
pmatcomp=pmatrixcompare
proglist="pdotproduct psumscalar psumvector counting padding false1 pmatrixcompare pmatrixmult"
# added 'r00c0' to count the number of instructions
nhmBadEvents="r00c0"
#nhmBadFSEvents2="r0824,r10b0,r010f,r0127,r02a2,r04b0,r08f0,r04f1,r01f3,r80b0,r08b0,r02f0,04f3"
events=$nhmBadEvents
PERF=/usr/bin/perf
#path to the Dynamorio library
DYNAMORIO=${HOME}/umbra_test/packages/DynamoRIO-Linux-2.1.0-4/bin64
#path to the umbra package
UMBRA=${HOME}/umbra_test/packages/umbra
#threadList="1 2 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 16"
threadList="1 2 4 8"
NlistMatrix="300 600 900 1200 1500 1800"
NlistMatrix2="2000 4000 6000 8000 10000 12000"
NlistArray="1000000 2000000 5000000 10000000 20000000 50000000 100000000"
NlistScalar=$NlistArray
Slist1="31"   # strides
Slist2="31 150"
CC=gcc
R=1
for prog in $proglist
do
    SRC=$prog.c
    EXEGOOD=$prog-good
    EXEBAD_MA=$prog-badma
    EXEBAD_FS=$prog-badfs
    StrideFlag=1
    SList=$Slist1
    if [ $prog = "pmatrixmult" ]
    then
        type="MATRIX"
        NList=$NlistMatrix
        SList=
        StrideFlag=0
    elif [ $prog = "pmatrixcompare" ]
    then
        type="MATRIX"
        NList=$NlistMatrix2
        SList=
        StrideFlag=0
    elif [ $prog = "false1" ] || [ $prog = "psumscalar" ] || [ $prog = "padding" ]
    then
        type="SCALAR"
        NList=$NlistScalar
        StrideFlag=0
    else                                        #pdot, psumv, count have 1-D arrays
        type="ARRAY"
        NList=$NlistArray
    fi
    echo "# Running: prog=$prog : type=$type : ==========================================="
    #echo "#Running $CFILE on $machine"
    for N in $NList
    do
        if [ $prog = "pmatrixmult" ] ; then
            R=
        elif [ $prog = "pmatrixcompare" ] ; then
            R=5
        elif [ $N -ge 50000000 ] ; then	# Repeat factor is low (50)
            R=50
        else						# Repeat factor should be higher
            R=100
        fi
        #R=1
        echo "# Running: prog=$prog: N=$N : R=$R : ++++++++++++++++++++++++++++++++++++++++++++"
        MCMODEL=
        # use -mcmodel=large with gcc when statically allocating large arrays > 2 GB
        if [ $N -gt 100000000 ] && [ $type = "ARRAY" ]
        then
            MCMODEL="-mcmodel=large"
            # exec time doesn't seem to get affected by this, but anyway...
            # echo "MCMODEL=$MCMODEL, N=$N, type=$type, NList=$NList"
        fi
        for numThreads in $threadList
        do
            echo "# prog=$prog: N=$N : R=$R : numThreads=$numThreads : ---------------------"
            echo "#"
            gcc -DGOOD -DN=$N -DREPEAT=$R $SRC $MCMODEL -lpthread -lrt -o $EXEGOOD
            #/usr/bin/time -f "# Good: Elapsed_time1(s)=%e" ./$EXEGOOD $numThreads > /dev/null
            #sudo $PERF stat -x : -r 3 -e $events
            #sudo $PERF stat -r 3 -e $events ./$EXEGOOD $numThreads
            #./$EXEGOOD $numThreads
            $DYNAMORIO/drrun -ops "-thread_private -max_bb_instrs 512 -no_finite_bb_cache -no_finite_trace_cache" -client $UMBRA/build/bin/libfsd.so 0 "" ./$EXEGOOD $numThreads
            cat *.proc.log
            rm umbra.*
            sudo $PERF stat -x : -r 3 -e $events ./$EXEGOOD $numThreads
            rm ./$EXEGOOD
            echo "#"
            gcc -DBAD_FS -DN=$N -DREPEAT=$R $SRC $MCMODEL -lpthread -lrt -o $EXEBAD_FS
            #./$EXEBAD_FS $numThreads
            #./$EXEBAD_FS $numThreads
            $DYNAMORIO/drrun -ops "-thread_private -max_bb_instrs 512 -no_finite_bb_cache -no_finite_trace_cache" -client $UMBRA/build/bin/libfsd.so 0 "" ./$EXEBAD_FS $numThreads
            cat *.proc.log
            rm umbra.*
            sudo $PERF stat -x : -r 3 -e $events ./$EXEBAD_FS $numThreads
            rm ./$EXEBAD_FS
            echo "#"
        done
    done
done
