/*
 * readwrite - In this program some threads will read while others will write
 *
 * compile as: gcc -DGOOD -DN=x -DREPEAT=y readwrite.c -lpthread -lrt -o readwrite 
 * 
 * run as: ./matadd <num_threads> 
 * 
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
#define 	REPEAT	10			// to repeat iterations 
#endif
#define	CACHELINE 64				// size of cacheline is 64 bytes
#define	DATASIZE  4				// int = 4 bytes (long long = 8 bytes)
#define MAXTHREADS CACHELINE/DATASIZE		// max # parallel threads to sum (with false sharing)
#define 	GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
						{ perror("clock_gettime( ):"); exit(EXIT_FAILURE); }
						
#ifdef 	GOOD
#define 	MSG 	"# ReadWrite: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# ReadWrite: Bad-FS : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif

float elapsed_time_msec(struct timespec *, struct timespec *, long *, long *);	// proto type
void *compute(void *vargp);

// Global shared variables
int count[MAXTHREADS];
int numthreads;

int main(int argc, char **argv) 
{
	int 			i, result = 0;
	pthread_t 		main_tid, tid[MAXTHREADS];
	int 			myid[MAXTHREADS], status;
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

	for (i=0; i< MAXTHREADS; i++) 			//get a random value to each element in count[i]
	  count[i] = 1;
	
	GET_TIME(t1);		
  	
    	for (i = 0; i < numthreads; i++) {  	                
		myid[i] = i;
		count[i] = 0;
		pthread_create(&tid[i], NULL, compute, &myid[i]); 
    	}                                                 
    	for (i = 0; i < numthreads; i++)  {                  
		pthread_join(tid[i], NULL);                   
    	}

	GET_TIME(t2);

  	comp_time = elapsed_time_msec(&t1, &t2, &sec, &nsec);
  	total_time = elapsed_time_msec(&t0, &t2, &sec, &nsec);
	#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# PReadWrite: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n", \
			N, numthreads, comp_time, 100.0*comp_time/total_time);
		return 0;
	}
	#endif
	printf(MSG, N, numthreads, comp_time, 100.0*comp_time/total_time);
    	//pthread_exit(NULL);
    	return 0;
}

void * compute(void *p) 
{
    	int 		myid = *((int *)p);            
    	int 		start = (myid * (long long)N)/numthreads;		 
    	int 		end = ((myid+1) * (long long)N) /numthreads;	  
    	int		i, j, k=myid,localcount=0,globalcount=0;
    	int	 	mycount[MAXTHREADS]; mycount[myid]=1;
	
	for(j=0;j<REPEAT;j++)
	{
	  #ifdef GOOD
	  if(myid % 2 == 0)
	    for(i=start;i<end;i++)
	      localcount += mycount[myid];
	    
	  else
	    for(i=start;i<end;i++)
	      mycount[myid] = i;	 
	  
	  #elif defined BAD_FS
	  if(myid % 2 == 0)
	    for(i=start;i<end;i++)
	      globalcount += count[myid];
	    
	  else
	    for(i=start;i<end;i++)
	      count[myid] = i;	      
	  #endif
	}
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
	return (float) (*sec) * 1000 + ((float) (*nsec)) / 1000000;

}
	
