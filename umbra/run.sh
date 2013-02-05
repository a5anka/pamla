#!/bin/bash
#This script runs streamcluster program with different no of threads, display the values in the proc.log and display the instruction count using perf tool.
#The data size has to be passed as a command line argument.
#   eg: run.sh "10 20 128 16384 16384 1000"

for i in 1 2 4 8 12
do
    echo "------------------------------------------------------------------------------"
    echo "No of Threads: $i"
    ../DynamoRIO-Linux-2.1.0-4/bin64/drrun -ops "-thread_private -max_bb_instrs 512 -no_finite_bb_cache -no_finite_trace_cache" -client ./build/bin/libfsd.so 0 "" streamcluster $1 none output.txt $i
    cat *.proc.log
# Remove the generated logs.
    rm umbra.streamcluster*
    perf stat -e r00c0 streamcluster $1 none output.txt $i 
done
