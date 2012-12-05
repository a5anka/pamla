#!/usr/bin/env bash

JAVA_PROGRAMS="Read DotProduct Write"
JAVA_SOURCE_PATH="java"
repeat=7

#nhmBadMAEvents="r0149,r0151,r02a2"
#nhmBadFSEvents="r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
# added 'r00c0' instructions retired to use for normalization
nhmBadEvents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
#nhmBadFSEvents2="r0824,r10b0,r010f,r0127,r02a2,r04b0,r08f0,r04f1,r01f3,r80b0,r08b0,r02f0,04f3"
events=$nhmBadEvents

#PERF=/usr/bin/perf_3.2
PERF=/usr/bin/perf
STRIDELIST="31"
STRIDELIST2="17 31 127 8191"
STRIDELIST3="64 128 256 512 1024 2048 4096 8192"

# on commit*, no difference among linear, random, strided versions until N=524288 (2^19)=> 4MB data
start=1048576   # = 2^20;  was 524288 = 2^19
mult=2
end=134217728 # 268435456	# = 2^28; (was 134217728 = 2^27)
# with doubles (8 byte), data size = 4KB; up to 32 KB fits in L1 DCache
# for doubles (8 byte), e.g. size of each Array=8* 2^^28=8*268,435,456=8* 256*1024*1024 =>2GB
#

i=$start
while [ $i -le $end ]
do
    for PROGRAM in $JAVA_PROGRAMS
    do
        echo "# Running $PROGRAM with N=$i ---------------------------------------"

        pushd $JAVA_SOURCE_PATH > /dev/null
        EXE=${PROGRAM}Linear2
        SOURCE=${EXE}.java
        echo "PROGRAM=$PROGRAM:N=$i:ALGO=LINEAR2:TYPE=GOOD"
        javac $SOURCE
        # sudo $PERF stat -r 3 -x : -e $events java -Xmx2048m -ea $EXE $i $repeat  > /dev/null
        java -Xmx2048m -ea $EXE $i $repeat
        rm -f $EXE.class
        echo "#"

        EXE=${PROGRAM}Random2
        SOURCE=${EXE}.java
        echo "PROGRAM=$PROGRAM:N=$i:ALGO=Random2:TYPE=BAD"
        javac $SOURCE
        # sudo $PERF stat -r 3 -x : -e $events java -Xmx2048m -ea $EXE $i $repeat > /dev/null
        java -Xmx2048m -ea $EXE $i $repeat 
        rm -f $EXE.class
        echo "#"

        for j in $STRIDELIST
        do
            EXE=${PROGRAM}Stride
            SOURCE=${EXE}.java
            echo "PROGRAM=$PROGRAM:N=$i:ALGO=Stride-$j:TYPE=BAD"
            javac $SOURCE
            # sudo $PERF stat -r 3 -x : -e $events java -Xmx2048m -ea $EXE $i $repeat $j > /dev/null
            java -Xmx2048m -ea $EXE $i $repeat $j 
            rm -f $EXE.class
            echo "#"
        done
        popd > /dev/null
    done
    i=$(( $i * $mult ))
done
