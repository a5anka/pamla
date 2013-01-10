/************************************************************************
 * counting.c - A simple counting parallel program to demo false sharing
 *
 *  Compilation: gcc -DN=<array_size> -DREPEAT=<n> -D[GOOD|BAD] counting.c \
 *               -lpthread -lrt -o count
 *
 * Execution: ./count <numthreads>
 ************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#ifndef N
#define N 100000000
#endif

#ifndef THREADS
#define THREADS 4
#endif

#ifndef REPEAT
#define REPEAT 5
#endif

#define CACHELINE 64 // size of cacheline is 64 bytes
#define DATASIZE 4 // int=4 bytes, long long is 8 bytes
#define MAXTHREADS CACHELINE/DATASIZE // max # parallel threads to sum (with false sharing)
#define GET_TIME(x); if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
                        { perror("clock_gettime( ):"); exit(EXIT_FAILURE); }

#ifndef STRIDE
#define STRIDE 127
#endif

#define GOOD 0
#define BADFS 1
#define BADMA 2

void *pcountGood(void *vargp);
void *pcountBadFS(void *vargp);
void *pcountBadMA(void *vargp);
float elapsed_time_msec(struct timespec *, struct timespec *, long *, long *);  // proto type

// Global shared variables
int count[MAXTHREADS];
int x[N];
int totalcount=0, numthreads;

int main(int argc, char **argv)
{
   int                  i, j, result = 0;
   pthread_t            main_tid, tid[MAXTHREADS];
   int                  myid[MAXTHREADS], status;
   struct timespec      t0, t1, t2, t3;
   unsigned long        sec, nsec;
   float                comp_time, total_time;  // in milli seconds
   time_t ltime;
   char *typeName;


   GET_TIME(t0);
   if (argc != 2) {
      printf("Usage: %s <num_threads> (max threads = 16)\n", argv[0]);
      exit(0);
   }
   numthreads = atoi(argv[1]);
   if (numthreads > MAXTHREADS) {
      printf("nthreads > MAXTHREADS (16)\n");
      exit(0);
   }

   for (j = 0; j < 9; ++j)
      {
         int type = j % 3;
         if (type == GOOD)
            {
               typeName = "GOOD";
            }
         else if (type == BADFS)
            {
               typeName = "BADFS";
            }
         else
            {
               typeName = "BADMA";
            }
         ltime=time(NULL);

         printf ("%d. Starting %s phase at %s", j + 1, typeName,
                 asctime(localtime(&ltime)) );
         for (i=0; i < N; i++)
            x[i] = random();          // not optimized

         GET_TIME(t1);
         for (i = 0; i < numthreads; i++) {
            myid[i] = i;
            count[i] = 0;

            if (type == GOOD)
               {
                  pthread_create(&tid[i], NULL, pcountGood, &myid[i]);
               }
            else if (type == BADFS)
               {
                  pthread_create(&tid[i], NULL, pcountBadFS, &myid[i]);
               }
            else
               {
                  pthread_create(&tid[i], NULL, pcountBadMA, &myid[i]);
               }
         }

         for (i = 0; i < numthreads; i++)  {
            pthread_join(tid[i], NULL);
         }

         GET_TIME(t2);
         for (i=0; i < numthreads; i++)
            result += count[i];

         if (result != totalcount) printf("# Error! : result(%d) != totalcount(%d)\n", result, totalcount);
         GET_TIME(t3);
         comp_time = elapsed_time_msec(&t1, &t2, &sec, &nsec);
         total_time = elapsed_time_msec(&t0, &t3, &sec, &nsec);

         if (type == BADFS && numthreads == 1)
            {
               printf("# PCount: Good   : N=%d : Threads=%d : CompTime(ms)=%.2f : CompTime/TotalTime=%.1f%%\n", \
                      N, numthreads, comp_time, 100.0*comp_time/total_time);
               continue;
            }

         printf ("Time: %f\n", comp_time);
         //printf(MSG, N, numthreads, comp_time, 100.0*comp_time/total_time);
      }
   return 0;
}
//------------------------------------------
void * pcountGood(void *p)
{
   int          myid = *((int *)p);
   int          start = (myid * (long long)N)/numthreads;
   int          end = ((myid+1) * (long long)N) /numthreads;
   int          i, j, k=myid, n=1, len;
   int          mycount[MAXTHREADS]; mycount[myid]=0;

   for (j = 0; j < REPEAT; ++j) {
      for (i = start; i < end; i++) {
         n += (start + (i * STRIDE + k))% 2;
         if (x[i] % 2 == 0)
            mycount[myid]++;
      }
   }
   count[myid] = mycount[myid];
   mycount[myid] = n;
   totalcount += count[myid];           // ideally should use locks
}

// BAD FS version
void * pcountBadFS(void *p)
{
   int          myid = *((int *)p);
   int          start = (myid * (long long)N)/numthreads;
   int          end = ((myid+1) * (long long)N) /numthreads;
   int          i, j, k=myid, n=1, len;
   int          mycount[MAXTHREADS]; mycount[myid]=0;

   for (j = 0; j < REPEAT; ++j) {
      for (i = start; i < end; i++) {
         n += (start + (i * STRIDE + k))% 2;
         if (x[i] % 2 == 0)
            count[myid]++;      // causes false sharing among threads
      }
   }
   mycount[myid] = n;
   totalcount += count[myid];           // ideally should use locks
}

// BAD MA version
void * pcountBadMA(void *p)
{
   int          myid = *((int *)p);
   int          start = (myid * (long long)N)/numthreads;
   int          end = ((myid+1) * (long long)N) /numthreads;
   int          i, j, k=myid, n=1, len;
   int          mycount[MAXTHREADS]; mycount[myid]=0;

   for (j = 0; j < REPEAT; ++j) {
      len = end - start;
      for (k=0; k < STRIDE; k++) {      // bad memory access (strided)
         for (i = 0; i < len/STRIDE; i++) {
            n = start + (i * STRIDE + k) ;
            if (x[n] % 2 == 0)
               mycount[myid]++;
         }
      }
      if ((len/STRIDE)*STRIDE != len) { // do the remainder, if any
         for (n=start + (len/STRIDE)*STRIDE; n < end; n++) { // linearly
            if (x[n] % 2 == 0)
               mycount[myid]++;
         }
      }
   }
   count[myid] = mycount[myid];
   mycount[myid] = n;
   totalcount += count[myid];           // ideally should use locks
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
