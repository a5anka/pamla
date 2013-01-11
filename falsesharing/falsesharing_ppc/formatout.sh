#!/usr/bin/env bash
LINES=19

sed -e '/^#.*[^%]$\|^#$/d' ppc_perf_out_2.log | \



awk -F : '{
if ($6 ~ "Comp.*")
  print $1 "," $2
else 
  print $3
} ' | \

awk "ORS=NR%${LINES}?\",\":\"\n\"" |\

cut -d , -f 5-
