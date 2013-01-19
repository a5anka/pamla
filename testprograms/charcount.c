/*
 * charcount - This program will produce the count of each character
 *
 * compile as: gcc -DGOOD -DN=x -DREPEAT=y charcount.c -lpthread -lrt -o charcount 
 * 
 * run as: ./charcount <num_threads> 
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>	// for clock_gettime()
#include <errno.h>	// for perror()

#ifndef 	N
#define 	N 1000000   // size of char array
#endif
#ifndef 	REPEAT
#define 	REPEAT	10			// to repeat iterations 
#endif
#define	CACHELINE 64				// size of cacheline is 64 bytes
#define	DATASIZE  8				// char = 1 byte + padding + int = 8 bytes
#define MAXTHREADS CACHELINE/DATASIZE		// max # parallel threads to sum (with false sharing)
#define 	GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
						{ perror("clock_gettime( ):"); exit(EXIT_FAILURE); }

#ifdef 	GOOD
#define 	MSG 	"# charcount: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef 	BAD_MA
#define 	MSG 	"# charcount: Bad-MA : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# charcount: Bad-FS : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifndef 	STRIDE
#define	STRIDE	31
#endif

//function prototype
void *count(void *vargp);
void initialize();
float elapsed_time_msec(struct timespec *, struct timespec *, long *, long *);	

// Global shared variables
char	data[N];
struct key {
	char letter;
	int count;
} keytab[] = {
	'a',0,
	'b',0,
	'c',0,
	'd',0,
	'e',0,
	'f',0,
	'g',0,
	'h',0,
};
int numthreads;

int main(int argc, char **argv) 
{
  pthread_t 		tid[MAXTHREADS];  
  int 			i,j,myid[MAXTHREADS],result;
  struct timespec 	t0, t1, t2, t3;
  unsigned long 	sec, nsec;
  float 		comp_time, total_time; 	// in milli seconds
  
  GET_TIME(t0);
  
  if (argc != 2) {				// if number of threads not specified
		printf("Usage: %s <numthreads>\n", argv[0]);
		exit(0);
    	}

  numthreads = atoi(argv[1]);
  
  if (numthreads > MAXTHREADS) {		//number of threads specified larger than MAXTHREADS
		printf("numthreads > MAXTHREADS (%d)\n", MAXTHREADS);
		exit(0);
    	}
    	
  initialize();
    	
 GET_TIME(t1);
  
  for (i = 0; i < numthreads; i++) {
		myid[i] = i;
		pthread_create(&tid[i], NULL, count, &myid[i]); 
    	}                                                 
  for (i = 0; i < numthreads; i++)  {                  
		pthread_join(tid[i], NULL);                   
    	}
    	
  GET_TIME(t2); 
  for (i=0; i < numthreads; i++)
		result += keytab[i].count;
  
	//if (result != (REPEAT * N)) printf("# Error! : result(%d) != correct(%d)\n", result, REPEAT * N);
	GET_TIME(t3);
  	comp_time = elapsed_time_msec(&t1, &t2, &sec, &nsec);
  	total_time = elapsed_time_msec(&t0, &t3, &sec, &nsec);
	#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# charcount: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n", \
			N, numthreads, comp_time, 100.0*comp_time/total_time);
		return 0;
	}
	#endif
	printf(MSG, N, numthreads, comp_time, 100.0*comp_time/total_time);
  	return 0;
}

void initialize()
{
  int i,a = 97, count = 0;
  
  for(i = 0; i < N; i++){
     data[i] = a + count;
      count ++;
      
      if(count == 8)
	count = 0;
    }
}
void *count(void *p){
      	int 		myid = *((int *)p);  
    	int 		start = (myid * (long long)N)/numthreads;
    	int 		end = ((myid+1) * (long long)N) /numthreads; 
    	int		i, j, k, n=1, len, constant = 97;
    	int	 	mycount[MAXTHREADS]; mycount[myid]=0;
  
  for(j = 0;j < REPEAT; j++){
	#ifdef GOOD
	for (i = start; i < end; i++) {
			if (data[i] == (myid + constant))
    				mycount[myid]++;	//increments a local array mycount
    		}
	#elif defined BAD_MA
	len = end - start;
    		for (k=0; k < STRIDE; k++) {		// bad memory access (strided)
   			for (i = 0; i < len/STRIDE; i++) {
    				n = start + (i * STRIDE + k) ;
    				if (data[i] == (myid + constant))
    					mycount[myid]++;
    			}
    		}
    		if ((len/STRIDE)*STRIDE != len) {	
    			for (n=start + (len/STRIDE)*STRIDE; n < end; n++) { 
    				if (data[i] == (myid + constant))
    				mycount[myid]++;  				
    			}
		}
	#elif defined BAD_FS
	for (i = start; i < end; i++) {
			if (data[i] == (myid + constant))
    				keytab[myid].count++;	//increments a global structure count
    		}
	#endif
  }
	#ifndef BAD_FS
	keytab[myid].count = mycount[myid];
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
