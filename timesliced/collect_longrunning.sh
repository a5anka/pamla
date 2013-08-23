#!/usr/bin/env bash

perfEvents="r0149,r0151,r20f0,r04b8"

program="dd"

pid=$(pidof $program)

echo "pid is ${pid}"

while [ ${pid} ]
do
    echo -n "# Capture time : "
    date
    perf stat -x : -e $perfEvents -p $pid sleep 0.000000001

    echo
done

