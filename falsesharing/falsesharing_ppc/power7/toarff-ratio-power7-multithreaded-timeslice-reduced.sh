#!/usr/bin/env bash

DATAFILE="/home/sunimal/Dropbox/FYP/PPC/Power7/outputfiles/falcon_timesliced_multithreaded_training.out"
NUMBER_PERFEVENTS=8

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_training_data_for_power7_multithreaded
%
%       9 attributes for each data instance
%

@attribute r3c046 numeric
@attribute r2c048 numeric
@attribute r2f080 numeric
@attribute r26880 numeric
@attribute rd0a0 numeric
@attribute status {good, badfs, badma}
@data'

sed -e 's/<not counted>/0/g' "$DATAFILE" | \

awk -F : '{
if ($1 ~ "\#")
  if($2 ~ "Good")
    print "good"
  if($2 ~ "Bad-FS")
    print "badfs"
  if($2 ~ "Bad-MA")
    print "badma"
if ($1 !~ "\#")
  print $1
} ' | \

 awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

 awk -F , -v OFS=, -v OFMT="%.4f" '{print $4/$9*10^9,$5/$9*10^9,$6/$9*10^9,$7/$9*10^9,$8/$9*10^9,$1}'
