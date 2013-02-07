#!/bin/sh
#
#	run.sh
#
#	As “perf” sends output to “stderr”, collect data onto a file “myfile.dat” as follows
#	shell%>./run-read.sh  &> myfile.dat
#	Try on snb (t420):
#	sudo perf stat -e r003c,r00c0,r4f2e,r412e,r0451,r0148,r04d2,r02d2,r08d2,r04d1,r0851 ./psum-2-pad 2 28
#	Try on nhm (commit5):
#	sudo perf stat -e r003c,r00c0,r4f2e,r412e,r020f,r080f,r0126,r0f26,r0851,r08cb ./psum 2 28
#
pmat=pmatrixmult
pdot=pdotproduct
psums=psumscalar
psumv=psumvector
count=counting
pad=padding
false1=false1
pmatcomp=pmatrixcompare

#padding program removed from program list since perf gives some invalid output when it is executed.
proglist="pdotproduct psumscalar psumvector counting false1 pmatrixcompare pmatrixmult"
#proglist="pdotproduct psumscalar psumvector counting padding false1 pmatrixcompare pmatrixmult"
#proglist="padding false1 pmatrixcompare pmatrixmult"
#nhmBadMAEvents="r0149,r0151,r02a2"
#nhmBadFSEvents="r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
# added 'r00c0' instructions retired to use for normalization
nhmBadEvents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
#Following line has been modified to remove r0700 event which is highly inconsistant
ppc970BadEvents="r1426,r0813,r50cb,r0704,r5001,r0705,r0935,r504b,r1800,r2837,r442d,r0800,r0925,r0320,instructions"
ppc7BadEvents="r30083,r4d048,r30064,r200fe,rd0a0,rd8ac,rd0a1,rd0a2,r4003e,rd0ac,r26880,r3c05c,r26182,r400f0,r1c048,r10081,r10083,r26080,r200f6,rd0aa,r1c04e,r4f080,rd0a8,r300f6,r2d05c,r20016,r1d048,rd8a8,r16080,r2,r3d05c,r1d04e"
#ppcBadEvents="r1426,r0813,r50cb,r0704,r5001,r0705,r0935,r504b,r1800,r2837,r442d,r0800,r0925,r0320,r0700,instructions"
#nhmBadFSEvents2="r0824,r10b0,r010f,r0127,r02a2,r04b0,r08f0,r04f1,r01f3,r80b0,r08b0,r02f0,04f3"
events=$ppc7BadEvents
PERF=/usr/bin/perf
#threadList="1 2 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 16"
threadList="1 2 4 8 16 32"
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
            sudo $PERF stat -r 3 -e $events ./$EXEGOOD $numThreads
            rm ./$EXEGOOD
            echo "#"
            gcc -DBAD_FS -DN=$N -DREPEAT=$R $SRC $MCMODEL -lpthread -lrt -o $EXEBAD_FS
            #./$EXEBAD_FS $numThreads
            #./$EXEBAD_FS $numThreads
            sudo $PERF stat -r 3 -e $events ./$EXEBAD_FS $numThreads
            rm ./$EXEBAD_FS
            echo "#"
            if [ $prog = "pmatrixcompare" ] ; then
                gcc -DBAD_MA -DN=$N -DREPEAT=$R $SRC $MCMODEL -lpthread -lrt -o $EXEBAD_MA
                #/usr/bin/time -f "# Bad: Elapsed_time1(s)=%e" ./$EXEBAD_MA $numThreads > /dev/null
                #./$EXEBAD_MA $numThreads
                sudo $PERF stat -r 3 -e $events ./$EXEBAD_MA $numThreads
                rm ./$EXEBAD_MA
                echo "#"
            fi
            if [ $StrideFlag -eq 0  ]	# if [ $StrideFlag -eq 0 ],  [ !$StrideFlag ]
            then
                continue
            fi
            for S in $SList
            do
                echo "#         STRIDE=$S : ......................"
                #gcc -DGOOD2 -DN=$N -DREPEAT=$R -DSTRIDE=$S -DCHECK $SRC $MCMODEL -lpthread -lrt -o $EXEGOOD-2
                #gcc -DGOOD2 -DN=$N -DREPEAT=$R -DSTRIDE=$S $SRC $MCMODEL -lpthread -lrt -o $EXEGOOD-2
                #/usr/bin/time -f "# Bad: Elapsed_time1(s)=%e" ./$EXEBAD $numThreads > /dev/null
                #./$EXEGOOD-2 $numThreads
                #rm ./$EXEGOOD-2
                gcc -DBAD_MA -DN=$N -DREPEAT=$R -DSTRIDE=$S $SRC $MCMODEL -lpthread -lrt -o $EXEBAD_MA
                #/usr/bin/time -f "# Bad: Elapsed_time1(s)=%e" ./$EXEBAD_MA $numThreads > /dev/null
                #./$EXEBAD_MA $numThreads
                sudo $PERF stat -r 3 -e $events ./$EXEBAD_MA $numThreads
                rm ./$EXEBAD_MA
                echo "#"
            done
        done
    done
done
