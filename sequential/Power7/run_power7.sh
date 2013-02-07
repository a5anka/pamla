#!/usr/bin/env bash

###################################################################
#
# To measure time of multiple commands using /usr/bin/time, try:
#   /usr/bin/time sh -c 'cmd1 > /dev/null ; cmd2 > /dev/null'
# can replace ' and && can replace ;
#
###################################################################

PROGS="read readmodifywrite write dotproduct swap matrixmultiply"
CC=gcc
type=double
repeat=7

#nhmBadMAEvents="r0149,r0151,r02a2"
#nhmBadFSEvents="r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
# added 'r00c0' instructions retired to use for normalization
ppc7BadEvents="r30083,r4d048,r30064,r200fe,rd0a0,rd8ac,rd0a1,rd0a2,r4003e,rd0ac,r26880,r3c05c, r26182, r400f0, r1c048, r10081, r10083, r26080, r200f6, rd0aa, r1c04e, r4f080, rd0a8, r300f6, r2d05c, r20016, r1d048, rd8a8, r16080, r2, r3d05c, r1d04e"
#nhmBadFSEvents2="r0824,r10b0,r010f,r0127,r02a2,r04b0,r08f0,r04f1,r01f3,r80b0,r08b0,r02f0,04f3"
events=$nhmBadEvents

PERF=/usr/local/bin/perf
STRIDELIST="31"
STRIDELIST2="17 31 127 8191"
STRIDELIST3="64 128 256 512 1024 2048 4096 8192"

# on commit*, no difference among linear, random, strided versions until N=524288 (2^19)=> 4MB data
start=1048576   # = 2^20;  was 524288 = 2^19
mult=2
end=268435456	# = 2^28; (was 134217728 = 2^27)
# with doubles (8 byte), data size = 4KB; up to 32 KB fits in L1 DCache
# for doubles (8 byte), e.g. size of each Array=8* 2^^28=8*268,435,456=8* 256*1024*1024 =>2GB
#

i=$start
while [ $i -le $end ]
do
    #SOURCE=$PROG.c
    #EXE=$PROG
    #echo "# Running $PROG=========================================================="
    #i=$start
    #while [ $i -le $end ]
    #echo "# Running with N=$i =========================================================="
    for PROG in $PROGS
    do
        SOURCE=$PROG.c
        EXE=$PROG
        MCMODEL=
        if [ $i -gt 100000000 ] # && [ $PROG = "dotproduct" ]
        then	# when memory >= 2GB will need to compiled with "-mcmodel=large" flag
            MCMODEL="-mcmodel=large"
        fi

        #echo "# Running $PROG=========================================================="
        echo "# Running $PROG with N=$i : MCMODEL=<$MCMODEL>---------------------------------------"
        #gcc -DN=$i -DREPEAT=$repeat -DLINEAR1 -DWARMUP -DTYPE=$type $SOURCE $MCMODEL -lrt -o $EXE-linear
        #gcc -DN=$i -DREPEAT=$repeat -DLINEAR1 -DTYPE=$type $SOURCE $MCMODEL -lrt -o $EXE-linear
        #sudo $PERF stat -r 3 -x : -e $events  ./$EXE-linear
        #/usr/bin/time -f "# Linear: Elapsed_time1(s)=%e" ./$EXE-linear
        #/usr/bin/time -f "# Linear: Elapsed_time1(s)=%e" sh -c 'cmd1 > /dev/null ; cmd2 > /dev/null'
        #./$EXE-linear
        #rm -f $EXE-linear
        #echo "#"
        echo "PROGRAM=$PROG:N=$i:ALGO=LINEAR2:TYPE=GOOD"
        gcc -DN=$i -DREPEAT=$repeat -DLINEAR2 -DTYPE=$type $SOURCE $MCMODEL -lrt -o $EXE-linear2
        #./$EXE-linear2
        sudo $PERF stat -r 3 -e $events  ./$EXE-linear2 > /dev/null
        rm -f $EXE-linear2
        echo "#"
        #echo "#ooooooooooo Linear-W-Opt"
        #gcc -DN=$i -DREPEAT=$repeat -DWARMUP -DLINEAR -DTYPE=$type -O3 $SOURCE $MCMODEL -lrt -o $EXE-linopt
        #echo "#"
        echo "PROGRAM=$PROG:N=$i:ALGO=RANDOM2:TYPE=BAD-MA"
        gcc -DN=$i -DREPEAT=$repeat -DRANDOM2 -DTYPE=$type $SOURCE $MCMODEL -lrt -o $EXE-random2
        #./$EXE-random2
        sudo $PERF stat -r 3 -e $events  ./$EXE-random2 > /dev/null
        rm -f $EXE-random2
        echo "#"
        #access with strides
        for j in $STRIDELIST
        do
            echo "PROGRAM=$PROG:N=$i:ALGO=STRIDE-$j:TYPE=BAD-MA"
            gcc -DN=$i -DREPEAT=$repeat -DSTRIDE=$j -DTYPE=$type $SOURCE $MCMODEL -lrt -o $EXE-stride$j
            #./$EXE-stride$j
            sudo $PERF stat -r 3 -e $events  ./$EXE-stride$j > /dev/null
            rm -f $EXE-stride$j
            echo "#"
        done
        #echo "#done N=$i============================================"
        #i=$(( $i * $mult ))
    done

    i=$(( $i * $mult ))
done
