#!/usr/bin/env bash

DATAFILE="/home/himeshi/Documents/FYP/project/timeslice/longrunning/linear_regression.out"
NUMBER_PERFEVENTS=4

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_data_for_reduced_events
%
%       16 attributes for each data instance
%
@attribute r0149 numeric
@attribute r0151 numeric
@attribute r20f0 numeric
@attribute r04b8 numeric
@attribute status {good, badfs, badma}
@data'

sed -e 's/#P/P/g' -e '/^$\|^Swaption\|^PARSEC\|^read\|^Number\|^yuv4mpeg\|^encoded/d' -e '/([0-9]*,[0-9]*)/d' -e 's/<not counted>/0/g' "$DATAFILE" | \

awk -F : '{
if ($1 !~ "#")
  print $1 
} '| \

awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F, -v OFS="," '
{
    print $2/$1*10^5, $3/$1*10^5, $4/$1*10^5, $5/$1*10^5, "?"
}'
