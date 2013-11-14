#!/usr/bin/env bash

DATAFILE="/home/sunimal/Dropbox/FYP/timesliced/data/0pical_timesliced_sao40.out"
NUMBER_PERFEVENTS=4


LINES=`expr $NUMBER_PERFEVENTS + 1 + 2`

echo '@relation testing_arff_for_timesliced_detection_nehalem_pical
%
%       4 attributes for each data instance
%
@attribute r0149 numeric
@attribute r0151 numeric
@attribute r20f0 numeric
@attribute r04b8 numeric
@attribute status {good, badfs, badma}
@data'
sed -e 's/#P/P/g' -e '/PID/d' -e '/#/d' -e '/ /d' -e '/---/d' -e '/Capture time/d' -e '/With*/d' -e '/   Our*/d' -e '/   Single*/d' -e '/                   pi*/d' -e '/10000000/d' -e '/No of */d'  -e 's/<not counted>/0/g' "$DATAFILE" |\
# 
#    Our
#    Single
#                    pi   
 
# if ($1 ~ "Capture time ")
#   print $2 ":" $3 ":" $4
# else
awk -F : '{

if ($1 ~ "ppical-good")
  print "good"
else
if ($1 ~ "ppical-bad_fs")
  print "badfs"
else
if ($2 ~ "r*")
  print $1  
} ' |\

# 
#sed -e '/#/d' -e 's/PROGRAM=\|INPUT=\|NTHREADS=//g' |\
#   
awk "ORS=NR%${LINES}?\",\":\"\n\"" |\
 
awk -F, -v OFS="," -v OFMT="%.4f" '
{ 
   print "% ID: " NR
   print $3, $4, $5, $6, $1
}'
