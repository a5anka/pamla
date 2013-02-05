#!/usr/bin/env bash

DATAFILE="/tmp/test.out"
NUMBER_PERFEVENTS=16

LINES=`expr $NUMBER_PERFEVENTS + 2`

echo '@relation performance_data_for_parsec_benchmark
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

sed -e 's/#P/P/g' -e '/^$\|^Swaption\|^PARSEC\|^read\|^Number\|^yuv4mpeg\|^encoded\|^ Performance/d' \
    -e '/([0-9]*,[0-9]*)/d' -e 's/<not counted>/0/g' -e 's/,//g' "$DATAFILE"  | \

awk 'BEGIN{
  FS="[ \t]+";
}
{
if ($0 !~ "PROGRAM.*")
  print $2
else
  print $1

}' | \

sed -e 's/PROGRAM=\|INPUT=\|NTHREADS=//g' -e 's/:/,/g' | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F, -v OFS="," -v OFMT="%.4f" '
{
    print "% Program: " $1
    print "% Input: " $2
    print "% Threads: " $3
    print "% Time: " $20 "s"
    print "% ID: " NR
    print $5/$4*10^9, $6/$4*10^9, $7/$4*10^9, $8/$4*10^9, $9/$4*10^9, $10/$4*10^9, $11/$4*10^9, $12/$4*10^9, $13/$4*10^9, $14/$4*10^9, $15/$4*10^9, $16/$4*10^9, $17/$4*10^9, $18/$4*10^9, $19/$4*10^9, "?"
}'
