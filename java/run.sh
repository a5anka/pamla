#!/bin/sh
#
#	run.sh
#
#	As “perf” sends output to “stderr”, collect data onto a file “myfile.dat” as follows
#	shell%>./run-read.sh  &> myfile.dat
#	Try on snb (t420):
#	sudo perf stat -e r003c,r00c0,r4f2e,r412e,r0451,r0148,r04d2,r02d2,r08d2,r04d1,r0851 ./psum-2-pad 2 28
#	Try on nhm (commit5):
#	sudo perf stat -e r003c,r00c0,r4f2e,r412e,r020f,r080f,r0126,r0f26,r0851,r08cb ./psum 2 28
#
pmat=pmatrixmult
pdot=pdotproduct
psums=psumscalar
psumv=psumvector
count=counting
pad=padding
false1=false1
pmatcomp=pmatrixcompare
proglist="psumscalar pdotproduct pmatrixmult"
#proglist="padding false1 pmatrixcompare pmatrixmult"
#nhmBadMAEvents="r0149,r0151,r02a2"
#nhmBadFSEvents="r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
# added 'r00c0' instructions retired to use for normalization
nhmBadEvents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"
#nhmBadFSEvents2="r0824,r10b0,r010f,r0127,r02a2,r04b0,r08f0,r04f1,r01f3,r80b0,r08b0,r02f0,04f3"
events=$nhmBadEvents
PERF=/usr/bin/perf
#threadList="1 2 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 16"
#threadList="1 3 6 9 12"
threadList="1 2 3 4"
NlistMatrix="300 600 900 1200 1500 1800"
NlistMatrix2="2000 4000 6000 8000 10000 12000"
NlistArray="1000000 2000000 5000000 10000000 20000000 50000000 100000000"
NlistScalar=$NlistArray
Slist1="31"   # strides
Slist2="31 150"
CC=gcc
R=1
REPEAT=3
STRIDE=31
for prog in $proglist
do
    SRC=$prog
    EXEGOOD=$prog-good
    EXEBAD_MA=$prog-badma
    EXEBAD_FS=$prog-badfs
    StrideFlag=1
    SList=$Slist1
    if [ $prog = "pmatrixmult" ]
    then
        type="MATRIX"
        NList=$NlistMatrix
        SList=
        StrideFlag=0
    elif [ $prog = "pmatrixcompare" ]
    then
        type="MATRIX"
        NList=$NlistMatrix2
        SList=
        StrideFlag=0
    elif [ $prog = "false1" ] || [ $prog = "psumscalar" ] || [ $prog = "padding" ]
    then
        type="SCALAR"
        NList=$NlistScalar
        StrideFlag=0
    else                                        #pdot, psumv, count have 1-D arrays
        type="ARRAY"
        NList=$NlistArray
    fi
    
    for N in $NList
    do
	for numThreads in $threadList
	do
	    if [ $prog = "pdotproduct" ] || [ $prog = "pmatrixmult" ] 
	    then
		echo "# Running: prog=$prog : type=GOOD : ==========================================="
		progname=$prog"_GOOD"
		javac $progname".java"
		sudo $PERF stat -x : -r 3 -e $events java -Xmx6g $progname $numThreads $N $REPEAT $STRIDE
		
		echo "# Running: prog=$prog : type=BAD-MA : ==========================================="
		progname=$prog"_BAD_MA"
		javac $progname".java"
		sudo $PERF stat -x : -r 3 -e $events java -Xmx6g $progname $numThreads $N $REPEAT $STRIDE
		
		echo "# Running: prog=$prog : type=BAD-FS : ==========================================="
		progname=$prog"_BAD_FS"
		javac $progname".java"
		sudo $PERF stat -x : -r 3 -e $events java -Xmx6g $progname $numThreads $N $REPEAT $STRIDE
		
		rm *.class
	    fi
	    
	    if [ $prog = "psumscalar" ]
	    then
		echo "# Running: prog=$prog : type=GOOD : ==========================================="
		progname=$prog"_GOOD"
		javac $progname".java"
		sudo $PERF stat -x : -r 3 -e $events java -Xmx6g $progname $numThreads $N $REPEAT
		
		echo "# Running: prog=$prog : type=BAD-FS : ==========================================="
		progname=$prog"_BAD_FS"
		javac $progname".java"
		sudo $PERF stat -x : -r 3 -e $events java -Xmx6g $progname $numThreads $N $REPEAT
		
		rm *.class
	    fi
	    
	done
    done
done
