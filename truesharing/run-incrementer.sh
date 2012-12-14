#!/usr/bin/env bash

PROGRAM=incrementer
SOURCE=${PROGRAM}.c
NUMBER_LIST="16 32" 
REPEAT_LIST="10000 100000 1000000" 
THREAD_LIST="2 4"

for N in $NUMBER_LIST; do
    for REPEAT in $REPEAT_LIST; do
        echo "Running with N=${N}:REPEAT=${REPEAT}----------------"
        for THREAD in $THREAD_LIST; do
            echo "THREADS:$THREAD"
            echo "PROGRAM=$PROGRAM:N=$N:TYPE=GOOD"
            gcc -lrt -lpthread -DTHREAD=$THREAD -DN=$N -DREPEAT=$REPEAT -DGOOD -o ${PROGRAM}-GOOD $PROGRAM.c
            ./${PROGRAM}-GOOD
            rm ${PROGRAM}-GOOD
            
            echo "PROGRAM=$PROGRAM:N=$N:TYPE=BAD"
            gcc -lrt -lpthread -DTHREAD=$THREAD -DN=$N -DREPEAT=$REPEAT -DBAD -o ${PROGRAM}-BAD $PROGRAM.c
            ./${PROGRAM}-BAD
            rm ${PROGRAM}-BAD
            echo
        done
    done
done
