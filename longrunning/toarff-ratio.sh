#!/usr/bin/env bash

DATAFILE="/tmp/test.out"
NUMBER_PERFEVENTS=16

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_data_for_long_running_program
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
@attribute status {good, badfs, badma}
@data'

sed  -e '/^$\|^Colecting/d' -e '/([0-9]*,[0-9]*)/d' -e 's/<not counted>/0/g' "$DATAFILE"  | \

awk -F : '{
if ($0 ~ "^# .*")
  print $0
else
  print $1
} ' | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F, -v OFS="," -v OFMT="%.4f" '
{
    print "% " $1
    print "% ID: " NR
    print $3/$2*10^9, $4/$2*10^9, $5/$2*10^9, $6/$2*10^9, $7/$2*10^9, $8/$2*10^9, $9/$2*10^9, $10/$2*10^9, $11/$2*10^9, $12/$2*10^9, $13/$2*10^9, $14/$2*10^9, $15/$2*10^9, $16/$2*10^9, $17/$2*10^9, "?"
#    print $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17
}'
