#!/usr/bin/env bash

sed -e '/^#\|^PROGRAM/d' -e "s/N=\|DataSize=\|Elapsed(msec)=//g" -e "s/:/,/g" java.out 
