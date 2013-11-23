! /bin/bash

#must be placed inside the same directory with compiled binaries of pical program
#
PERF=perf
data=(10000000 100000000)
#data=(1000 10000 100000 1000000 10000000)
#power7reducedevents="r3c046,r2c048,r2f080,r26880,rd0a0,r2"
power7reducedevents="r3c046,r26880,rd0a0,r2"
nehalemevents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
nehalemReducedEvents="r00c0,r0149,r0151,r20f0,r04b8"
events=$nehalemReducedEvents
PROGRAM=(ppical-good ppical-bad_fs)

for i in 1 2 4 8 12
do
    echo "------------------------------------------------"
    echo "No of threads ${i}"
    for d in ${data[@]}; do
        echo $d
        for program in ${PROGRAM[@]}; do

            ./$program $i $d &
            PID=$(pidof $program)
            while test -d /proc/$PID; do
                echo $program
                echo "PID is $PID"
                echo -n "Capture time : "
                date
                sudo $PERF stat -x : -e ${events} -p $PID sleep 0.0001
                echo
            done
        done
    done
    echo
done
