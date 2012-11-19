/* 
 * padding.c - A simple parallel program to demo false sharing (this uses padding to avoid false sharing)
 */

//	compile as: gcc -DN=1000000 -D[GOOD|BAD] padding.c -lpthread -lrt -o padding
//		
//	run as: ./padding <numthreads>	


#include <pthread.h>	
#include <stdio.h>
#include <stdlib.h>	// for atoi() etc
#include <time.h>		// for clock_gettime()
#include <errno.h>	// for perror()

#ifndef 	N
#define 	N		100000000
#endif
#ifndef 	REPEAT
#define 	REPEAT	1
#endif
#define	INTSIZE	4
#define	CACHELINE	64
#define 	PADDING 	(CACHELINE - INTSIZE)
#define 	STATEBUFSZ 	8	// STATEBUFSZ=8 works; 16 OK/NOT and 32 NOT/OK for commit*/T420
#define 	MAXTHREADS 	CACHELINE/INTSIZE
#define 	GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
						{ perror("clock_gettime( ):"); exit(EXIT_FAILURE); }
#ifdef 	GOOD
#define 	MSG 	"# Padding: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# Padding: Bad-FS : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif

void *thread_func(void *p);
float elapsed_time_msec(struct timespec *, struct timespec *, long *, long *);	// proto type


typedef struct {
   	int x;
	#ifdef GOOD
   	char padding[PADDING];
   	#endif
} dtype;

// Global shared variables
dtype 	*sdata; 
int 		numthreads;

int main(int argc, char **argv) 
{
	int 				i, j;
	pthread_t 			tid[MAXTHREADS];
	int 				myid[MAXTHREADS];
	struct timespec 		t0, t1, t2, t3;
	unsigned long 		sec, nsec;
	float 			comp_time, total_time; 	// in milli seconds
	
	GET_TIME(t0);
    	if (argc != 2) {
		printf("Usage: %s <num_threads> (max threads = 16)\n", argv[0]);
		exit(0);
    	}
    	numthreads = atoi(argv[1]);
    	if (numthreads > MAXTHREADS) {
		printf("numthreads > MAXTHREADS (16)\n");
		exit(0);
    	}
    	sdata = (dtype *) malloc(sizeof(dtype)*numthreads);
	for (i=0; i < numthreads; i++) {
		(sdata[i]).x = 0;
	}

	GET_TIME(t1);
  	//total_time = elapsed_time_msec(&t0, &t1, &sec, &nsec);
  	//printf("InitTime(ms)=%8.1f: ", total_time);
    	// Create peer threads and wait for them to finish
    	for (i = 0; i < numthreads; i++) {
    		myid[i] = i;                  
		pthread_create(&tid[i], NULL, thread_func, (void *) &myid[i]); 
    	}                                                 
    	for (i = 0; i < numthreads; i++)  {                  
		pthread_join(tid[i], NULL);                   
    	}
	GET_TIME(t2);
	free(sdata);
	GET_TIME(t3);
  	comp_time = elapsed_time_msec(&t1, &t2, &sec, &nsec);
  	total_time = elapsed_time_msec(&t0, &t3, &sec, &nsec);
	#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# Padding: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n", \
			N, numthreads, comp_time, 100.0*comp_time/total_time);
		return 0;
	}
	#endif
	printf(MSG, N, numthreads, comp_time, 100.0*comp_time/total_time);
	return 0; 
}
//------------------------------------------ 	
void * thread_func(void *p) 
{
    	int 		myid = *((int *)p);            
   	int 		i,j;
   	int 		mystart = (myid * (long long)N)/numthreads; 	
   	int 		myend =  ((myid+1) * (long long)N) /numthreads;	
   	
   	struct {
   		struct random_data rdata;
   		char rand_state[STATEBUFSZ];	// STATEBUFSZ=8 works ok on most systems; 16, 32 not on all
   	} tdata;
   	int32_t r;

   	initstate_r(1 /* seed */, tdata.rand_state, STATEBUFSZ, &(tdata.rdata));
    	for (i = 0; i < REPEAT; ++i) {
    		for (j=mystart; j < myend; j++) {
    			random_r(&(tdata.rdata), &r);
         		sdata[myid].x += j * (r/(RAND_MAX/2)) ; 
  		}
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
	

