#!/usr/bin/env bash

DATAFILE="/home/sunimal/Dropbox/FYP/timesliced/data/0sao40reducedtraining_multithreaded.out"
NUMBER_PERFEVENTS=4

LINES=`expr $NUMBER_PERFEVENTS + 1 + 3`

echo '@relation training_arff_for_timesliced_detection_nehalem
%
%       4 attributes for each data instance
%
@attribute r0149 numeric
@attribute r0151 numeric
@attribute r20f0 numeric
@attribute r04b8 numeric
@attribute status {good, badfs, badma}
@data'
sed -e 's/#P/P/g' -e 's/<not counted>/0/g' "$DATAFILE" |\

awk -F : '{
if ($4 ~ " Threads=1 ")
  print "good"
else
if ($2 ~ " Good   ")
  print "good"
else
if ($2 ~ " Bad-MA ")
  print "badma"
else
if ($2 ~ " Bad-FS ")
  print "badfs"  
else
  print $1
} ' | \

sed -e '/#/d' -e 's/PROGRAM=\|INPUT=\|NTHREADS=//g' |\
  
awk "ORS=NR%${LINES}?\",\":\"\n\"" |\
 
awk -F, -v OFS="," -v OFMT="%.4f" '
{ 
   print "% ID: " NR
   print $5/$4*10^9, $6/$4*10^9, $7/$4*10^9, $8/$4*10^9, $1
}'
