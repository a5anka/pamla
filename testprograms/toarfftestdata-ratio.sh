#!/usr/bin/env bash

NUMBER_PERFEVENTS=16

LINES=`expr $NUMBER_PERFEVENTS + 3`

echo '% ARFF file for charconvert2out.c
%
@relation bad_mem_access_and_false_sharing
%
%       16 attributes for each data instance
%
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
@attribute status {good, badfs, badma}
@data'

sed -e '/prog/d' -e '/^#$/d' -e 's/Good/good/g' -e 's/Bad-FS/badfs/g' -e 's/Bad-MA/badma/g' -e 's/<not counted>/0/g' charconvert2out.dat | \

awk -F : '{
if ($1 !~ "#")
  print $1
else
  print $2
} '  | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F, -v OFS="," -v OFMT="%.4f" '{
  print $5/$4*10^9, $6/$4*10^9, $7/$4*10^9, $8/$4*10^9, $9/$4*10^9, $10/$4*10^9, $11/$4*10^9, $12/$4*10^9, $13/$4*10^9, $14/$4*10^9, $15/$4*10^9, $16/$4*10^9, $17/$4*10^9, $18/$4*10^9, $19/$4*10^9, $1
} '

# #awk -F, -v OFS="," -v OFMT="%.4f" '
# #{
# #   print $2/$1*10^9"," $3/$1*10^9"," $4/$1*10^9"," $5/$1*10^9"," $6/$1*10^9"," $7/$1*10^9"," $8/$1*10^9"," $9/$1*10^9"," $10/$1*10^9"," $11/$1*10^9"," $12/$1*10^9"," $13/$1*10^9"," $14/$1*10^9"," $15/$1*10^9"," $16/$1*10^9"," $17
# #}'


# #
# ##awk -F "," '{
# #for (i=4; i <= 19; i++) {
# #		print $i","
# #	}
# #print $1
# #} '
