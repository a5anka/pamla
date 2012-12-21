#!/usr/bin/env bash

DATAFILE=data/intel-data/parsec.out
NUMBER_PERFEVENTS=16

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_data_for_sequential_memory_access
%
%       16 attributes for each data instance
%
@attribute r00c0 numeric
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
@attribute status {GOOD, BAD-FS, BAD-MA}
@data'

sed -e 's/#P/P/g' -e '/^$\|^Swaption\|^PARSEC\|^read\|^Number\|^yuv4mpeg\|^encoded/d' -e '/([0-9]*,[0-9]*)/d' -e 's/<not counted>/0/g' $DATAFILE | \

awk -F : '{
if ($1 !~ "PROGRAM.*")
  print $1
else 
  print $1 "," $2 "," $3
} ' | \

sed -e 's/PROGRAM=\|INPUT=\|NTHREADS=//g' | \
 
awk "ORS=NR%${LINES}?\",\":\"\n\"" | sed 's/$/,?/g' # | cut -d , -f 4-
