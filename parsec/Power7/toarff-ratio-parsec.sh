#!/usr/bin/env bash

DATAFILE="/home/sunimal/temp/power7/outparsec.dat"
NUMBER_PERFEVENTS=32

LINES=`expr $NUMBER_PERFEVENTS + 2`

echo '@relation performance_data_for_parsec_benchmark
%
% 32 attributes for each data instance
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

sed -e 's/#P/P/g' -e '/^$\|^Swaption\|^PARSEC\|^read\|^Number\|^yuv4mpeg\|^encoded\|^ Performance\|^(.*)/d' \
    -e 's/<not counted>/0/g' -e 's/,//g' "$DATAFILE" | \

awk 'BEGIN{
FS="[ \t]+";
}
{
if ($0 !~ "PROGRAM.*")
print $2
else
print $1

}' |\

sed -e 's/PROGRAM=\|INPUT=\|NTHREADS=//g' -e 's/:/,/g' |\

awk "ORS=NR%${LINES}?\",\":\"\n\"" |\

awk -F, -v OFS="," -v OFMT="%.4f" '
{
if ($33 == 0) {
    NR--;
} else {
    print "% Program: " $1
    print "% Input: " $2
    print "% Threads: " $3
    print "% Time: " $36 "s"
    print "% ID: " NR

    print $4/$33*10^9, $5/$33*10^9, $6/$33*10^9, $7/$33*10^9, $8/$33*10^9, $9/$33*10^9, $10/$33*10^9, $11/$33*10^9, $12/$33*10^9, $13/$33*10^9, $14/$33*10^9, $15/$33*10^9, $16/$33*10^9, $17/$33*10^9, $18/$33*10^9, $19/$33*10^9, $20/$33*10^9, $21/$33*10^9, $22/$33*10^9, $23/$33*10^9, $24/$33*10^9, $25/$33*10^9, $26/$33*10^9, $27/$33*10^9, $28/$33*10^9, $29/$33*10^9, $30/$33*10^9, $30/$33*10^9, $32/$33*10^9, $34/$33*10^9, $35/$33*10^9, "?"
}
}'
