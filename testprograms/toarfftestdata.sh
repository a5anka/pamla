#!/usr/bin/env bash

NUMBER_PERFEVENTS=16

LINES=`expr $NUMBER_PERFEVENTS + 3`

echo '% ARFF file for charcount.c
%
@relation performance_data_for_test_program
%
%       16 attributes for each data instance
%
@attribute status {good, badfs}
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

sed -e '/prog/d' -e '/^#$/d' -e 's/Good/good/g' -e 's/Bad-FS/badfs/g' -e 's/Bad-MA/badma/g' -e 's/<not counted>/?/g' charcount.dat| \

awk -F : '{
if ($1 !~ "#")
  print $1
else 
  print $2
} ' | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F "," '{
print $5", " $6", " $7", " $8", " $9", " $10", " $11", " $12", " $16", " $14", " $15", " $16", " $17", " $18", " $19", "$1
} '

