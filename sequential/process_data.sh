#!/usr/bin/env bash

SOURCEDIR=$(pwd)
FILENAME=plot_data
CSVFILE=data_csv
DATACOLLECT_SCRIPT=$SOURCEDIR/collect_plot_data.sh
PLOTTING_SCRIPT=$SOURCEDIR/plot_csv.py

# $DATACOLLECT_SCRIPT > $FILENAME

awk -F : '{print $1 "-" $2 "-" $3 "," $4 "," $5 "," $6}' $FILENAME | \
sed 's/[ #]\|N=\|DataSize=\|Elapsed(msec)=//g' > $CSVFILE

$PLOTTING_SCRIPT

# Clean the directory
rm $FILENAME $CSVFILE
