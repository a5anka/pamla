#!/usr/bin/env bash

DATAFILE=data/intel-data/parsec-with_O_level2.out
NUMBER_PERFEVENTS=16

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_data_for_parsec_banchmark
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

sed -e 's/#P/P/g' -e '/^$\|^Swaption\|^PARSEC\|^read\|^Number\|^yuv4mpeg\|^encoded/d' -e '/([0-9]*,[0-9]*)/d' -e 's/<not counted>/0/g' $DATAFILE | \

awk -F : '{
if ($1 !~ "PROGRAM.*")
  print $1
else 
  print $1 "," $2 "," $3
} ' | \

sed -e 's/PROGRAM=\|INPUT=\|NTHREADS=//g' | \
 
awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F, -v OFS="," '
{
    print "% Program: " $1 
    print "% Input: " $2 
    print "% Threads: " $3
    print "% ID: " NR
    print $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, "?"
}'
