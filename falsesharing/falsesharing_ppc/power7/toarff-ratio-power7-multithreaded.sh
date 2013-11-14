#!/usr/bin/env bash

DATAFILE="/home/sunimal/Dropbox/FYP/PPC/Power7/outputfiles/falcon_multi_threaded_training.out"
NUMBER_PERFEVENTS=12

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_training_data_for_power7_multithreaded
%
%       9 attributes for each data instance
%

@attribute r3c046 numeric
@attribute r2c048 numeric
@attribute r2f080 numeric
@attribute r26080 numeric
@attribute r30881 numeric
@attribute r26182 numeric
@attribute r26880 numeric
@attribute rd0a2 numeric
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

 awk -F , -v OFS=, -v OFMT="%.4f" '{print $4/$13*10^9,$5/$13*10^9,$6/$13*10^9,$7/$13*10^9,$8/$13*10^9,$9/$13*10^9,$10/$13*10^9,$11/$13*10^9,$12/$13*10^9,$1}'
