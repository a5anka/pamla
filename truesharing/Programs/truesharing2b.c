/* 
 * truesharing2.c - A simple shared variable program to demo true sharing
 * 
 */

//	compile as: gcc -DN=<array_size> -DREPEAT=<n> -D[GOOD|BAD] truesharing2.c -lpthread -lrt -o truesharing
//		
//	run as: ./truesharing <numthreads>

    
#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>     
#include <semaphore.h> 
#include <time.h>		// for clock_gettime()

#ifndef 	N
#define 	N		10000000
#endif

#ifndef 	REPEAT
#define 	REPEAT	1	// to repeat iterations (for better data collection)
#endif
#define 	MAXTHREADS 	2			// max # parallel threads to sum (with false sharing)	
#define 	GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
						{ perror("clock_gettime( ):"); exit(EXIT_FAILURE); }
#ifdef 	GOOD
#define 	MSG 	"# PTrueSharing: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef	BAD_TS
#define 	MSG 	"# PTrueSharing: Bad-TS : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif

//prototypes
void handler (  void *p );
float elapsed_time_msec(struct timespec *, struct timespec *, long *, long *);


sem_t mutexA;
sem_t mutexB;
int counter1 = 0, counter2 = 0; /* shared variable */

int main(int argc, char **argv)
{
    int totalcount=0, numthreads, i;
    pthread_t 	main_tid, tid[MAXTHREADS];
    int 	myid[MAXTHREADS];
    struct timespec 	t0, t1, t2, t3;
    unsigned long 	sec, nsec;
    float 		comp_time, total_time; 	// in milli seconds
    
    	GET_TIME(t0);
    	if (argc != 2) {			//if the number of threads in not specified
		printf("Usage: %s <num_threads> (max threads = %d)\n", argv[0],MAXTHREADS);
		exit(0);
    	}

    	numthreads = atoi(argv[1]);		//convert number of threads into int

    	if (numthreads > MAXTHREADS) {		//print number of threads
		printf("nthreads > MAXTHREADS (16)\n");
		exit(0);
    	}
    
    sem_init(&mutexA, 0, 1);      
    sem_init(&mutexB, 0, 1); 
    
    GET_TIME(t1);
    
    for (i = 0; i < numthreads; i++) {  	                
	myid[i] = i;
	pthread_create(&tid[i], NULL, (void *) &handler, &myid[i]); 
    }   
    
    for (i = 0; i < numthreads; i++)  {                  
	pthread_join(tid[i], NULL);                   
    }
    
    GET_TIME(t2);	
                                                              
    sem_destroy(&mutexA);
    sem_destroy(&mutexB);
    
    if(counter1 != counter2){
       printf("# Error! : counter1(%d) != counter2(%d) \n", counter1, counter2);
    }
    
    GET_TIME(t3);
    
    comp_time = elapsed_time_msec(&t1, &t2, &sec, &nsec);
    total_time = elapsed_time_msec(&t0, &t3, &sec, &nsec);
    
     printf(MSG, N, numthreads, comp_time, 100.0*comp_time/total_time);
                   
    return 0;
} 

void handler ( void *p )
{
  int 	myid = *((int *)p);  
  int i, j, k;
  int localcounter1 = 0, localcounter2 = 0;
  
  
    #ifdef GOOD
  
    
      for(i = 0; i < REPEAT; i++){
      for( j = 0; j < N; j++ ){
	localcounter1++;
      }
    } 
     
       for(i = 0; i < REPEAT; i++){
      for( j = 0; j < N; j++ ){
	localcounter2++;
      }
    } 
 
    sem_wait(&mutexA);  
      counter1 = localcounter1;
       sem_post(&mutexA);  
       
       sem_wait(&mutexB);
      counter2 == localcounter2;
       sem_post(&mutexB);
       
    
    #elif defined BAD_TS
    
    for(i = 0; i < REPEAT; i++){
      for( j = 0; j < N; j++ ){
	sem_wait(&mutexA);  
      counter1++;
       sem_post(&mutexA);  
      }
    } 
    
        for(i = 0; i < REPEAT; i++){
      for( j = 0; j < N; j++ ){
       sem_wait(&mutexB);
      counter2++;
       sem_post(&mutexB);
      }
    } 

    #endif
    
    pthread_exit(0); /* exit thread */
}

//------------------------------------------ 	
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
	return (float) (*sec) * 1000 + ((float) (*nsec)