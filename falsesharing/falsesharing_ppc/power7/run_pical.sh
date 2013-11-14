#! /bin/bash
#
PERF=perf
data=(10000000 100000000)
#data=(1000 10000 100000 1000000 10000000)
power7reducedevents="r3c046,r2c048,r2f080,r26080,r30881,r26182,r26880,rd0a2,rd0a0"
nehalemevents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
events=$power7reducedevents
PROGRAM=(./ppical-good ./ppical-bad_fs)

for i in 1 2 4 8
do
    echo "------------------------------------------------"
    echo "No of threads ${i}"
    for d in ${data[@]}; do
        echo $d
        for program in ${PROGRAM[@]}; do
            echo $program 
            sudo $PERF stat -x : -r 3 -e ${events} $program $i $d
            echo
        done
    done
    echo
done