#!/usr/bin/env bash

DATAFILE=/home/sunimal/Dropbox/FYP/PPC/Power7/outputfiles/falcon_sequential_training.out
NUMBER_PERFEVENTS=10

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_data_for_sequential_memory_access_power7_platform
%
%       9 attributes for each data instance
%
@attribute status {GOOD, BAD-MA}
@attribute r3c046 numeric
@attribute r2c048 numeric
@attribute r2f080 numeric
@attribute r26080 numeric
@attribute r30881 numeric
@attribute r26182 numeric
@attribute r26880 numeric
@attribute rd0a2 numeric
@attribute rd0a0 numeric
@data'

sed -e '/^#/d' -e 's/TYPE=\([^:]*\)/\1/g' $DATAFILE | \

awk -F : '{
if ($1 !~ "PROGRAM.*")
  print $1
else 
  if ($4 ~ "BAD-MA")
    print "badma"
  else
    print "good"
} ' | \
 
 awk "ORS=NR%${LINES}?\",\":\"\n\"" | awk -F , -v OFS=, -v OFMT="%.4f" '{print $2/$11*10^9,$3/$11*10^9,$4/$11*10^9,$5/$11*10^9,$6/$11*10^9,$7/$11*10^9,$8/$11*10^9,$9/$11*10^9,$10/$11*10^9,$1}'
 #'{print $2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$1}'
