#!/usr/bin/env bash

#This script is used to run programs in PHOENIX benchmark with "Umbra" client
PH_DIR=/home/gayashan/umbra_test/packages/phoenix2_modified/tests
DYNAMORIO_LIB=/home/gayashan/umbra_test/packages/DynamoRIO-Linux-2.1.0-4/bin64/drrun
UMBRA_DIR=/home/gayashan/umbra_test/packages/umbra

#Add the path to the umbra client here
UMBRA_CLIENT="${UMBRA_DIR}/build/bin/libfsd.so" 

declare -A packages
#programs for PHOENIX benchmark
packages["string_match"]="string_match"
#packages["word_count"]="word_count"
#packages["linear_regression"]="linear_regression"

#Uncomment the line below to enable data files for programs except word_count
inputs=(key_file_50MB.txt key_file_100MB.txt  key_file_500MB.txt)
#Uncomment the line below to enable data files for word_count program
#inputs=( word_10MB.txt  word_50MB.txt word_100MB.txt)

#List of No of threads
THREAD_LIST=(1 3 6)

for program in ${!packages[@]}; do
    datadir="${PH_DIR}/${program}/datafiles"
    programdir="${PH_DIR}/${program}/${program}-pthread"
            
    for input in ${inputs[@]}; do
        datafile="${datadir}/$input"
        for threads in ${THREAD_LIST[@]}; do
            echo "#PROGRAM=$program:INPUT=$input:NTHREADS=$threads"
            $DYNAMORIO_LIB -ops "-thread_private -max_bb_instrs 512 -no_finite_bb_cache -no_finite_trace_cache" -client $UMBRA_CLIENT 0 "" $programdir $datafile $threads > /dev/null 
            cat *.proc.log
            rm umbra.*
        done
    done
done
