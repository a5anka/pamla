#!/bin/sh6
#
#	run-matrixmult.sh

#Ref: run.sh on how to customize

PROG=matrixmultiply
CFILE=$PROG.c
EXE=$PROG
CC=gcc
type=double
#nhmBadMAEvents="r0149,r0151,r02a2"
#nhmBadFSEvents="r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
# added 'r00c0' instructions retired to use for normalization
nhmBadEvents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
#nhmBadFSEvents2="r0824,r10b0,r010f,r0127,r02a2,r04b0,r08f0,r04f1,r01f3,r80b0,r08b0,r02f0,04f3"
events=$nhmBadEvents
#PERF=/usr/bin/perf_3.2
PERF=/usr/bin/perf
#
echo "# Running $PROG =========================================================="
#
start=400		# data size = up to 32 KB can fit in L1 DCache
incr=100		# min data size= 8*16*16 = 2KB per matrix; 6KB (8KB) for 3 (4) matrices
#end=2048               # max data size= 8*2^14*2^14 = 2^31 = 2GB per matrix, too high 2^14 = 16,384
end=1700                # = 8*2^13*2^13 = 2^29 = 512MB per matrix, use this, 2^13 = 8192
i=$start

#
# Analysis of inner loops (numbers are per inner loop interation)
# assuming:	- cache line (block) size = 64 B, data (double) word size = 8 B, N is large
#			- A matrix row will not fit in L1 cache; all scalars in registers
# Version (class)	Loads		Stores	A_miss	B_miss	C_miss	Total_miss
# ---------------	-----		------	-------	------	------	----------
# ikj, kij (CB)		2	1		0		1/8		1/8		0.25
# ijk, jik (AB)		2	0		1/8		1		0		1.125
# jki, kji (CA)		2	1		1		0		1		2

while [ $i -le $end ]
do
    echo "# Running $PROG with N=$i =============================================="
    gcc -DN=$i -DTRANSPOSE -DTYPE=$type -DCHECK $CFILE -lrt -o $EXE-trans
    #./$EXE-trans
    sudo $PERF stat -r 3 -x : -e $events  ./$EXE-trans
    rm -f $EXE-trans
    echo "#"
    gcc -DN=$i -DIKJ -DTYPE=$type $CFILE -lrt -o $EXE-ikj
    #./$EXE-ikj
    sudo $PERF stat -r 3 -x : -e $events  ./$EXE-ikj
    rm -f $EXE-ikj
    echo "#"
    gcc -DN=$i -DKIJ -DTYPE=$type $CFILE -lrt -o $EXE-kij
    #./$EXE-kij
    sudo $PERF stat -r 3 -x : -e $events  ./$EXE-kij
    rm -f $EXE-kij
    echo "#"
    #
    gcc -DN=$i -DJKI -DTYPE=$type $CFILE -lrt -o $EXE-jki
    #./$EXE-jki
    sudo $PERF stat -r 3 -x : -e $events  ./$EXE-jki
    rm -f $EXE-jki
    echo "#"
    gcc -DN=$i -DKJI -DTYPE=$type $CFILE -lrt -o $EXE-kji
    #./$EXE-kji
    sudo $PERF stat -r 3 -x : -e $events  ./$EXE-kji
    rm -f $EXE-kji
    echo "#"
    #skip blocking if i <= 128
    #skip blocking if i <= 150
    if [ $i -le 300 ]
    then
        i=$(( $i + $incr ))
        continue
    fi
    #with blocking
    #bstart=32
    #bend=$(( $i / 2))
    #j=$bstart
    #while [ $j -le $bend ]
    for j in 100 200 250 300 400
    do
        if [ `expr $i % $j` -ne 0 ] || [ $i -eq $j ]
        then
            #j=$(( $j * 2 ))
            continue
        fi
        echo "#         With BLOCKING for N=$i, Block size=$j-------------------"
        #gcc -DN=$i -DBLOCK=$j -DJK__IKJ -DTYPE=$type $CFILE -lrt -o $EXE-jk--ikj
        #./$EXE-jk--ikj
        #sudo $PERF stat -r 3  -x : -e $events  ./$EXE-jk--ikj
        #rm -f $EXE-jk--ikj
        #echo "#"
        #gcc -DN=$i -DBLOCK=$j -DKJ__IKJ -DTYPE=$type $CFILE -lrt -o $EXE-kj--ikj
        #./$EXE-kj--ikj
        #sudo $PERF stat -r 3  -x : -e $events  ./$EXE-kj--ikj
        #rm -f $EXE-kj--ikj
        #echo "#"
        gcc -DN=$i -DBLOCK=$j -DKJ__IJK -DTYPE=$type $CFILE -lrt -o $EXE-kj--ijk
                #./$EXE-kj--ijk
        sudo $PERF stat -r 3 -x : -e $events  ./$EXE-kj--ijk
        rm -f $EXE-kj--ijk
        echo "#"
        gcc -DN=$i -DBLOCK=$j -DTJK__IJK -DTYPE=$type $CFILE -lrt -o $EXE-tjk--ijk
        #./$EXE-tjk--ijk
        sudo $PERF stat -r 3 -x : -e $events  ./$EXE-tjk--ijk
        rm -f $EXE-tjk--ijk
        echo "#"
        gcc -DN=$i -DBLOCK=$j -DTJKI_IJK -DTYPE=$type $CFILE -lrt -o $EXE-tjki--ijk
        #./$EXE-tjki--ijk
        sudo $PERF stat -r 3 -x : -e $events  ./$EXE-tjki--ijk
        rm -f $EXE-tjki--ijk
        echo "#"
        #gcc -DN=$i -DBLOCK=$j -DJKI_IKJ -DTYPE=$type $CFILE -lrt -o $EXE-jki-ikj
        #./$EXE-jki-ikj
        #rm -f $EXE-jki-ikj
        #echo "#"
        #gcc -DN=$i -DBLOCK=$j -DKJI_IKJ -DTYPE=$type $CFILE -lrt -o $EXE-kji-ikj
        #./$EXE-kji-ikj
        #rm -f $EXE-kji-ikj
        #echo "#"
        #j=$(( $j * 2 ))
    done
    #echo "#
    #echo "#done N=$i============================================"
    i=$(( $i + $incr ))
    #i=$(( $i * $incr ))
done
