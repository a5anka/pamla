/***********************************************************
 *
 * Increment elements in the vector by the number of threads
 *
 * Compiliation: gcc -DGOOD | -DBAD [-DN N] [-DREPEAT REPEAT] [-DTHREADS THREADS]-o incrementer incrementer.c
 * Execution: ./incrementer
 *
 ***********************************************************/

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "lib/time_measure.h"

#ifndef N
#define N 128 //134217728
#endif

#ifndef THREADS
#define THREADS 4
#endif

#ifndef REPEAT
#define REPEAT 10000000
#endif

int64_t vector[N];
int threadIds[THREADS];

void * increment(void *p);

int main(int argc, char *argv[])
{
   int i;
   pthread_t threads[THREADS];
   struct timespec start, end;

   for ( i = 0; i < N; ++i)
      {
         vector[i] = 0;
      }

   saveCurrentTime(&start);
   
   for (i = 0; i < THREADS; i++) {
      threadIds[i] = i;
      pthread_create(&threads[i], NULL, increment, &threadIds[i]);
   }

   for (i = 0; i < THREADS; i++)  {                  
      pthread_join(threads[i], NULL);                   
   }

   saveCurrentTime(&end);
   printf ("Elapsed time(msec): %.3f\n",elapsedTime(&start, &end));

   return EXIT_SUCCESS;
}

void * increment(void * p) 
{
   int i, j;
   int myid = *((int *)p);
   int start = (myid * (int64_t)N)/THREADS;
   int index = start;
   
#ifdef BAD
   for (i = 0; i < N; ++i)
      {
         for (j = 0; j < REPEAT; ++j)
            {
               ++vector[i];
            }
         index = (index + 1) % N;
      }
#endif

#ifdef GOOD
   for (i = 0; i < N; ++i)
      {
         for (j = 0; j < REPEAT; ++j)
            {
               ++vector[index];
            }
         index = (index + 1) % N;
      }
#endif

   return NULL;
}
