#!/usr/bin/env bash

PROGRAMS="read readmodifywrite write dotproduct swap"
CC=gcc
DATATYPE=double
REPEAT=7
STRIDELIST="31"
STRIDELIST2="17 31 127 8191"
start=1048576 	
mult=2
end=67108864

# with doubles (8 byte), data size = 4KB; up to 32 KB fits in L1 DCache
# for doubles (8 byte), e.g. size of each Array=8* 2^^28=8*268,435,456=8* 256*1024*1024 =>2GB
i=$start
while [ $i -le $end ]
do
    for PROG in $PROGRAMS
    do
	SOURCE=$PROG.c
	EXECUTABLE=$PROG
	
        MCMODEL=
	# if [ $i -gt 100000000 ] 
	# then	# when memory >= 2GB will need to compiled with "-mcmodel=large" flag
	#     MCMODEL="-mcmodel=large"
	# fi
	
	gcc -DN=$i -DREPEAT=$REPEAT -DLINEAR2 -DTYPE=$DATATYPE $SOURCE $MCMODEL -lrt -o $EXECUTABLE-linear2
	./$EXECUTABLE-linear2
	rm -f $EXECUTABLE-linear2
        
	gcc -DN=$i -DREPEAT=$REPEAT -DRANDOM2 -DTYPE=$DATATYPE $SOURCE $MCMODEL -lrt -o $EXECUTABLE-random2
	./$EXECUTABLE-random2
	rm -f $EXECUTABLE-random2
	
        # access with strides 
	for j in $STRIDELIST
	do
	    gcc -DN=$i -DREPEAT=$REPEAT -DSTRIDE=$j -DTYPE=$DATATYPE $SOURCE $MCMODEL -lrt -o $EXECUTABLE-stride$j
	    ./$EXECUTABLE-stride$j
	    rm -f $EXECUTABLE-stride$j
	done
    done
    i=$(( $i * $mult ))
done

