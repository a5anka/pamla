#!/usr/bin/env bash

DATAFILE="/home/sunimal/temp/power7/ppc7perf_multithread.out"
NUMBER_PERFEVENTS=32

LINES=`expr $NUMBER_PERFEVENTS + 1`

echo '@relation performance_data_for_parsec_banchmark
%
%       31 attributes for each data instance
%

@attribute r30083 numeric
@attribute r4d048 numeric
@attribute r30064 numeric
@attribute r200fe numeric
@attribute rd0a0 numeric
@attribute rd8ac numeric
@attribute rd0a1 numeric
@attribute rd0a2 numeric
@attribute r4003e numeric
@attribute rd0ac numeric
@attribute r26880 numeric
@attribute r3c05c numeric
@attribute r26182 numeric
@attribute r400f0 numeric
@attribute r1c048 numeric
@attribute r10081 numeric
@attribute r10083 numeric
@attribute r26080 numeric
@attribute r200f6 numeric
@attribute rd0aa numeric
@attribute r1c04e numeric
@attribute r4f080 numeric
@attribute rd0a8 numeric
@attribute r300f6 numeric
@attribute r2d05c numeric
@attribute r20016 numeric
@attribute r1d048 numeric
@attribute rd8a8 numeric
@attribute r16080 numeric
@attribute r3d05c numeric
@attribute r1d04e numeric

@attribute status {good, badfs, badma}
@data'

sed -e '/^#\|^#$\|time elapsed\|^$\|#%$/d' -e 's/<not counted>/0/g' "$DATAFILE" | \

awk 'BEGIN{
  FS="[ \t]+";
}
{
if ($2 ~ "Performance*"){
    print $6
  }
else
  print $2
}' |\
 
awk "ORS=NR%${LINES}?\",\":\"\n\"" | \

awk -F, -v OFS="," -v OFMT="%.4f" '
{
  if ((x=index($1,"-")) > 0) {
        mode = substr($1,x+1,length($1));
    }

    print "% Program: " $1 
    print $2/$31*10^9, $3/$31*10^9, $4/$31*10^9, $5/$31*10^9, $6/$31*10^9, $7/$31*10^9, $8/$31*10^9, $9/$31*10^9, $10/$31*10^9, $11/$31*10^9, $12/$31*10^9, $13/$31*10^9, $14/$31*10^9, $15/$31*10^9, $16/$31*10^9, $17/$31*10^9, $18/$31*10^9, $19/$31*10^9, $20/$31*10^9, $21/$31*10^9, $22/$31*10^9, $23/$31*10^9, $24/$31*10^9, $25/$31*10^9, $26/$31*10^9, $27/$31*10^9, $28/$31*10^9, $29/$31*10^9, $30/$31*10^9, $32/$31*10^9, $33/$31*10^9, mode
}'
