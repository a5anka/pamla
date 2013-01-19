/*
 * matrixaddition - This program will produce the addition of N matrices
 *
 * compile as: gcc -DGOOD -DN=x -DREPEAT=y matrixaddition.c -lpthread -lrt -o matadd 
 * 
 * run as: ./matadd <num_threads> where num_threads > 4
 * num_threads can be 4, 8, 12, 16
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>	// for clock_gettime()
#include <errno.h>	// for perror()

#ifndef 	N
#define 	N 1000000   // matrices to be added
#endif
#ifndef 	REPEAT
#define 	REPEAT	100			// to repeat iterations 
#endif
#define	CACHELINE 64				// size of cacheline is 64 bytes
#define	DATASIZE  4				// int = 4 bytes (long long = 8 bytes)
#define MAXTHREADS CACHELINE/DATASIZE		// max # parallel threads to sum (with false sharing)	
#define DIMENSION 4				// dimension of the array = sqrt(MAXTHREADS)
#define 	GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
						{ perror("clock_gettime( ):"); exit(EXIT_FAILURE); }
						
#ifdef 	GOOD
#define 	MSG 	"# MatrixAddition: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# MatrixAddition: Bad-FS : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif


//function prototypes
void *add(void *vargp);
float elapsed_time_msec(struct timespec *, struct timespec *, long *, long *);	

//global variables
int total[DIMENSION][DIMENSION];

int main(int argc, char **argv)
{
  pthread_t 		tid[MAXTHREADS];  
  int 			i,j, numthreads, myid[MAXTHREADS],rows, check = 1 ;
  struct timespec 	t0, t1, t2, t3;
  unsigned long 	sec, nsec;
  float 		comp_time, total_time; 	// in milli seconds
  
  GET_TIME(t0);
  
  if (argc != 2) {				// if number of threads not specified
		printf("Usage: %s <numthreads>\n", argv[0]);
		exit(0);
    	}

  numthreads = atoi(argv[1]);
  rows = numthreads/DIMENSION;
  
  if (numthreads > MAXTHREADS) {		//number of threads specified larger than MAXTHREADS
		printf("numthreads > MAXTHREADS (%d)\n", MAXTHREADS);
		exit(0);
    	}
    	
  GET_TIME(t1);
  
  for (i = 0; i < numthreads; i++) {
		myid[i] = i;
		pthread_create(&tid[i], NULL, add, &myid[i]); 
    	}                                                 
  for (i = 0; i < numthreads; i++)  {                  
		pthread_join(tid[i], NULL);                   
    	}
    	
  GET_TIME(t2);
	
  for( i = 0; i < rows; i++){
    for( j = 0; j < DIMENSION; j++){
      if(total[i][j] != N*REPEAT)
	check = 0;
    }    
  }
  if (check == 0) printf("# Error! Array elements do not add up\n");
    GET_TIME(t3);

  comp_time = elapsed_time_msec(&t1, &t2, &sec, &nsec);
  total_time = elapsed_time_msec(&t0, &t3, &sec, &nsec);
  
  #ifdef BAD_FS
  if (numthreads == 1) {
	printf("# MatrixAddition: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n", \
	N, numthreads, comp_time, 100.0*comp_time/total_time);
	return 0;
	}
  #endif
  
  printf(MSG, N, numthreads, comp_time, 100.0*comp_time/total_time);
  return 0;	
}

void *add(void *p)
{
  int 	i, j, myid = *((int *)p), row, column, mycount[MAXTHREADS]; mycount[myid]=0; 
  
  for( j = 0; j<REPEAT; j++){
    #ifdef GOOD
    row = (myid / DIMENSION);
    column = ((row+1)*DIMENSION) - myid -1;
    for( i = 0; i < N; i++){
      mycount[myid]++;
    }
    #elif defined BAD_FS
    row = (myid / DIMENSION);
    column = ((row+1)*DIMENSION) - myid -1;
    for( i = 0; i < N; i++){
      total[row][column]++;
    }
    #endif
  }
  
    #ifdef GOOD
    total[row][column] = mycount[myid];
    #endif  
}

float elapsed_time_msec(struct timespec *begin, struct timespec *end, long *sec, long *nsec)
{
	if (end->tv_nsec < begin->tv_nsec) {
		*nsec = 1000000000 - (begin->tv_nsec - end->tv_nsec);
    		*sec  = end->tv_sec - begin->tv_sec -1;
    	}
    	else {
		*nsec = end->tv_nsec - begin->tv_nsec;
    		*sec  = end->tv_sec - begin->tv_sec;
	}
	return (float) (*sec) * 1000 + ((float) (*nsec)) / 1000000;

}
