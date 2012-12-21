#!/usr/bin/env bash

NUMBER_PERFEVENTS=16

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_data_for_sequential_memory_access
%
%       16 attributes for each data instance
%
@attribute status {GOOD, BAD-MA}
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
@data'

sed -e '/^#/d' -e 's/TYPE=\([^:]*\)/\1/g' train_data | \

awk -F : '{
if ($1 !~ "PROGRAM.*")
  print $1
else 
  print $4
} ' | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" 
