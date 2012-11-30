#!/usr/bin/env bash

NUMBER_PERFEVENTS=16

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_data_for_sequential_memory_access
%
%       16 attributes for each data instance
%
@attribute status {GOOD, BAD}
@attribute r03c numeric
@attribute r1a2 numeric
@attribute r4f2e numeric
@attribute r412e numeric
@attribute r151 numeric
@attribute r451 numeric
@attribute r148 numeric
@attribute r108 numeric
@attribute r1008 numeric
@attribute r149 numeric
@attribute r1049 numeric
@attribute raf2 numeric
@attribute r7f1 numeric
@attribute r324 numeric
@attribute r80f0 numeric
@data'

sed -e '/^#/d' -e 's/TYPE=\([^:]*\)/\1/g' train_data | \

awk -F : '{
if ($1 !~ "PROGRAM.*")
  print $1
else 
  print $4
} ' | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" 
