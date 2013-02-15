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
matadd=matrixaddition
cconvert=charconvert
ccount=charcount
readwrite=readwrite
proglist="matrixaddition charconvert charcount readwrite"
nhmBadEvents="r00c0,r0149,r0151,r02a2,r0126,r0227,r0224,r08a2,r01b0,r20f0,r02f1,r01f2,r01b8,r02b8,r04b8,r40cb"	
events=$nhmBadEvents
PERF=/usr/bin/perf
threadList1="1 3 4 6 9 12 16"
threadlistmat="4 8 12 16"
NlistMatrix="300 600 900 1200 1500 1800"
NlistMatrix2="2000 4000 6000 8000 10000 12000"
NlistArray="1000000 2000000 5000000 10000000 20000000 50000000 100000000"
Slist1="31"   
R=10
CC=gcc
for prog in $proglist
do
    SRC=$prog.c
    EXEGOOD=$prog-good
    EXEBAD_MA=$prog-badma
    EXEBAD_FS=$prog-badfs
    StrideFlag=1
    SList=$Slist1

if [ $prog = "matrixaddition" ]
    then
        threadlist=$threadlistmat
	SList=$SList1
	StrideFlag=0
	Nlist=$NlistMatrix

elif [ $prog = "charconvert" ]
   then
	threadlist=$threadlist1
	SList=$SList1
	StrideFlag=0
	Nlist=$NlistMatrix

elif [ $prog = "readwrite" ]
   then
	threadlist=$threadlist1
	SList=$SList1
	StrideFlag=0
	Nlist=$NlistMatrix
elif [ $prog = "charcount" ]
  then
	threadlist=$threadlist1
	SList=$SList1
	StrideFlag=0
	Nlist=$NlistMatrix
fi

for N in $NList
	do
	MCMODEL=
        if [ $N -gt 100000000 ] && [ $type = "ARRAY" ]
        then
            MCMODEL="-mcmodel=large"
	fi
       
	for numThreads in $threadList
        	do
		echo "# prog=$prog: N=$N : R=$R : numThreads=$numThreads : ---------------------"
            	echo "#"
            	gcc -DGOOD -DN=$N -DREPEAT=$R $SRC $MCMODEL -lpthread -lrt -o $EXEGOOD
		sudo $PERF stat -x : -r 3 -e $events ./$EXEGOOD $numThreads
            	rm ./$EXEGOOD
            	echo "#"
            	gcc -DBAD_FS -DN=$N -DREPEAT=$R $SRC $MCMODEL -lpthread -lrt -o $EXEBAD_FS
		sudo $PERF stat -x : -r 3 -e $events ./$EXEBAD_FS $numThreads
            	rm ./$EXEBAD_FS
		if [ $prog = "charconvert" ] ; then
                gcc -DBAD_MA -DN=$N -DREPEAT=$R $SRC $MCMODEL -lpthread -lrt -o $EXEBAD_MA
 		sudo $PERF stat -x : -r 3 -e $events ./$EXEBAD_MA $numThreads
                rm ./$EXEBAD_MA
                echo "#"
		fi
		if [ $prog = "charcount" ] ; then
                gcc -DBAD_MA -DN=$N -DREPEAT=$R $SRC $MCMODEL -lpthread -lrt -o $EXEBAD_MA
 		sudo $PERF stat -x : -r 3 -e $events ./$EXEBAD_MA $numThreads
                rm ./$EXEBAD_MA
                echo "#"
		fi
          	done
	done
done

