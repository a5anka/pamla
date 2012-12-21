#!/usr/bin/env bash

DATAFILE=data/intel-data/parsec-time.out

sed -e 's/#P/P/g' -e '/^$\|^Swaption\|^PARSEC\|^read\|^Number\|^user\|^sys/d' -e '/([0-9]*,[0-9]*)/d' -e 's/<not counted>/0/g' -e 's/^real	//g' $DATAFILE | \

awk -F : '{
if ($1 !~ "PROGRAM.*")
  print $1
else 
  print $1 "," $2 "," $3
} ' | \

sed -e 's/PROGRAM=\|INPUT=\|NTHREADS=//g' | \
 
awk "ORS=NR%2?\",\":\"\n\""
