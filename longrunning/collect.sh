#!/usr/bin/env bash

perfEvents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"

echo -n "Enter PID: "
read PID

echo "PID is ${PID}"

for i in {1..10}; do
    echo -n "# Capture time : "
    date
    perf stat -x : -e $perfEvents -p $PID sleep 2

    echo
done
