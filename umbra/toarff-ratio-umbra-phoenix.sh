#!/usr/bin/env bash

DATAFILE=$1

LINES=22

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
    -e "/^Performance counter stats for/d" "$DATAFILE" | \

awk 'BEGIN{
  FS="[ \t]+";
  program="program"
  N=0
}
{
if ($0 ~ "^#PROGRAM=") {
  program=$0
  NR--
} else {
if (NR%21 == 1)
  print program

if ($0 ~ "^[0-9]")
  print $1
else
  print $0
}
}'  | \

sed -e 's/#PROGRAM=\|INPUT=\|NTHREADS=//g' -e 's/ //g' -e 's/:R=[0-9]*\|:----\+//g' -e 's/:/,/g'  | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F, -v OFS="," -v OFMT="%.4f" '
{
    print "% Program: " $1
    print "% N: " $2
    print "% Threads: " $3
    print "% Time: " $24 "s"
    print "% ID: " NR
    print $9/$8*10^9, $10/$8*10^9, $11/$8*10^9, $12/$8*10^9, $13/$8*10^9, $14/$8*10^9, $15/$8*10^9, $16/$8*10^9, $17/$8*10^9, $18/$8*10^9, $19/$8*10^9, $20/$8*10^9, $21/$8*10^9, $22/$8*10^9, $23/$8*10^9, $6/$7*1000
}'
