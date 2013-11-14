/*
*
*/
/* Includes */
#include <unistd.h>     
#include <sys/types.h>  
#include <errno.h>      
#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    
#include <string.h>     
#include <semaphore.h>  

#ifndef 	N
#define 	N		100000000
#endif

#ifndef 	REPEAT
#define 	REPEAT	1	// to repeat iterations (for better data collection)
#endif
#define	CACHELINE	64				// size of cacheline is 64 bytes
#define	DATASIZE	4				// int=4 bytes, long long is 8 bytes
#define 	MAXTHREADS 	CACHELINE/DATASIZE	// max # parallel threads to sum (with false sharing)	
#define 	GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
						{ perror("clock_gettime( ):"); exit(EXIT_FAILURE); }
#ifdef 	GOOD
#define 	MSG 	"# PCount: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef 	BAD_MA
#define 	MSG 	"# PCount: Bad-MA : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# PCount: Bad-FS : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifndef 	STRIDE
#define	STRIDE	127
#endif

void handler ( void *ptr );								//prototypes
float elapsed_time_msec(struct timespec *, struct timespec *, long *, long *);

// Global shared variables
sem_t mutex;
int count[MAXTHREADS];
int x[N];
int totalcount=0, numthreads;
int counter; 

int main(int argc, char **argv)
{
    pthread_t 		main_tid, tid[MAXTHREADS];
    
    GET_TIME(t0);
    if (argc != 2) {			//if the number of threads in not specified
    printf("Usage: %s <num_threads> (max threads = %d)\n", argv[0],MAXTHREADS);
    exit(0);
    }
    
    numthreads = atoi(argv[1]);		//convert number of threads into into
    
    if (numthreads > MAXTHREADS) {		//print number of threads
	printf("nthreads > MAXTHREADS (16)\n");
	exit(0);
    }
    	
    GET_TIME(t1);
    
    sem_init(&mutex, 0, 1);      /* initialize mutex to 1 - binary semaphore */
                                 /* second param = 0 - semaphore is local */
  	
    for (i = 0; i < numthreads; i++) {  	                
	myid[i] = i;
	count[i] = 0;
	pthread_create(&tid[i], NULL, (void *) &handler, &myid[i]); 
    }                                                 
    for (i = 0; i < numthreads; i++)  {                  
	pthread_join(tid[i], NULL);                   
    }
    	
    GET_TIME(t2);
				 
    sem_destroy(&mutex); /* destroy semaphore */
                   
    exit(0);
} 

void handler ( void *ptr )
{
    int x; 
    x = *((int *) ptr);
    printf("Thread %d: Waiting to enter critical region...\n", x);
    sem_wait(&mutex);       /* down semaphore */
    /* START CRITICAL REGION */
    printf("Thread %d: Now in critical region...\n", x);
    printf("Thread %d: Counter Value: %d\n", x, counter);
    printf("Thread %d: Incrementing Counter...\n", x);
    counter++;
    printf("Thread %d: New Counter Value: %d\n", x, counter);
    printf("Thread %d: Exiting critical region...\n", x);
    /* END CRITICAL REGION */    
    sem_post(&mutex);       /* up semaphore */
    
    pthread_exit(0); /* exit thread */
}
