/*
 * charconvert.c - This program will convert each element in a char array into the next char
 *
 * compile as: gcc -DGOOD -DN=x -DREPEAT=y charconvert.c -lpthread -lrt -o charconvert 
 * 
 * run as: ./charconvert <num_threads> 
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
#define	DATASIZE  1				// char = 1 byte
#define MAXTHREADS CACHELINE/DATASIZE		// max # parallel threads to sum (with false sharing)
#define 	GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
						{ perror("clock_gettime( ):"); exit(EXIT_FAILURE); }

#ifdef 	GOOD
#define 	MSG 	"# charconvert: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef 	BAD_MA
#define 	MSG 	"# charconvert: Bad-MA : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# charconvert: Bad-FS : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n"
#endif
#ifndef 	STRIDE
#define	STRIDE	31
#endif

//function prototype
void *convert(void *vargp);
void initialize();
char getnext(char current);
float elapsed_time_msec(struct timespec *, struct timespec *, long *, long *);	

// Global shared variables
char data[N][N];
int numthreads;

int main(int argc, char **argv) 
{
  pthread_t 		tid[MAXTHREADS];  
  int 			i,j,myid[MAXTHREADS];
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
		pthread_create(&tid[i], NULL, convert, &myid[i]); 
    	}                                                 
  for (i = 0; i < numthreads; i++)  {                  
		pthread_join(tid[i], NULL);                   
    	}
    	
  GET_TIME(t2); 
  
  comp_time = elapsed_time_msec(&t1, &t2, &sec, &nsec);
  total_time = elapsed_time_msec(&t0, &t2, &sec, &nsec);
  
   #ifdef BAD_FS
  if (numthreads == 1) {
	printf("# charconvert: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n", \
	N, numthreads, comp_time, 100.0*comp_time/total_time);
	return 0;
	}
  #endif
  
  printf(MSG, N, numthreads, comp_time, 100.0*comp_time/total_time);

  return 0;
}

void *convert(void *p){
  int i, j,k, myid = *((int *)p),count=0;
  int start = (myid * (long long)N)/numthreads;
  int end = ((myid+1) * (long long)N) /numthreads;
  
  for(j = 0; j < REPEAT; j++){
    #ifdef GOOD
    if(myid == (numthreads-1))
      end = N-1;
    
    for(i = start; i < end; i++){
      for(k = 0; k < N; k++){
	data[i][k] = getnext(data[i][k]);
      }
    }
    #elif defined BAD_MA
    if(myid == (numthreads-1))
    end = N-1;
    
    for(i = start; i < end; i++){
      for(k = 0; k < N; k++){
	data[k][i] = getnext(data[k][i]);
      }
    }
    #elif defined BAD_FS
    for(i = 0; i < N; i++){
      for(k = 0; k < N; k++){
	if(count % numthreads == myid)
	  data[i][k] = getnext(data[i][k]);
      }
    }
    #endif
  }
}

void initialize()
{
  int i,j,a = 97, count = 0;
  
  for(i = 0; i < N; i++){
    for(j = 0; j < N; j++)
    { data[i][j] = a + count;
      count ++;
      
      if(count == 26)
	count = 0;
      
    }
    }
}

char getnext(char current)
{
  int i = current;
  
  if( i == 122 )
    return 97;
  else
    return i+1;
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