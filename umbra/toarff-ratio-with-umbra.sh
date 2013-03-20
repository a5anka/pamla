#!/usr/bin/env bash

DATAFILE="/tmp/test.out"

LINES=23

echo '@relation umbra_perf_training_data
%
%       16 attributes for each data instance
%
@attribute r0149 numeric
@attribute r0151 numeric
@attribute r02a2 numeric
@attribute r0126 numeric
@attribute r0227 numeric
@attribute r0224 numeric
@attribute r08a2 numeric
@attribute r01b0 numeric
@attribute r20f0 numeric
@attribute r02f1 numeric
@attribute r01f2 numeric
@attribute r01b8 numeric
@attribute r02b8 numeric
@attribute r04b8 numeric
@attribute r40cb numeric
@attribute fsrate numeric
@data'

sed -e '/^$\|^#$\|^# Running\|type=/d' -e 's/<not counted>/?/g' \
    -e 's/,\|^# \|^[ ]\+\|^Total \(cache \(miss\|invalidation\)\|false share\): \|^num instr: //g' \
    -e "s/Performance counter stats for '\.\/\(.\+-\(good\|badfs\|badma\)\) [0-9]\+' (3 runs):/\1/g" "$DATAFILE" | \

awk 'BEGIN{
  FS="[ \t]+";
  program="program"
  N=0
}
{
if ($0 ~ "^prog=") {
  program=$0
  NR--
} else {
if (NR%22 == 1)
  print program

if ($0 ~ "^[0-9]")
  print $1
else
  print $0
}
}' | \

sed -e 's/prog=\|N=\|numThreads=//g' -e 's/ //g' -e 's/:R=[0-9]*\|:----\+//g' -e 's/:/,/g' | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F, -v OFS="," -v OFMT="%.4f" '
{
    print "% Program: " $1
    print "% N " $2
    print "% Threads: " $3
    print "% Type: " $8
    print "% ID: " NR
    print $10/$9*10^9, $11/$9*10^9, $12/$9*10^9, $13/$9*10^9, $14/$9*10^9, $15/$9*10^9, $16/$9*10^9, $17/$9*10^9, $18/$9*10^9, $19/$9*10^9, $20/$9*10^9, $21/$9*10^9, $22/$9*10^9, $23/$9*10^9, $24/$9*10^9, $6/$7*1000
}'
