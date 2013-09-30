/* File:     pth_pi.c
 * Purpose:  Try to estimate pi using the formula
 *   pi = 4*[1 - 1/3 + 1/5 - 1/7 + 1/9 - . . . ]
 *
 * Compile:  gcc -g -Wall -o ppical ppical.c -lm -lpthread
 * Run:      ./pth_pi <number of threads> <n>
 *
 */        

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define CACHELINE 64
#define DATASIZE 4
#ifndef REPEAT
#define REPEAT 10
#endif
#define MAXTHREADS CACHELINE/DATASIZE

const int MAX_THREADS = 1024;

long thread_count;
long long n;
double sum;
double psum[MAXTHREADS];

void* Thread_sum(void* rank);
void* Thread_sum_with_fs(void* rank);
/* Only executed by main thread */
void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
double Serial_pi(long long n);

int main(int argc, char* argv[]) {
   long       thread;  /* Use long in case of a 64-bit system */
   pthread_t* thread_handles;
    #ifdef BAD_FS
    int i;
    #endif
   /* Get number of threads from command line */
   Get_args(argc, argv);

   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t)); 
   sum = 0.0;
   
   #ifdef BAD_FS
   for (thread = 0; thread < thread_count; thread++)  
      pthread_create(&thread_handles[thread], NULL,
          Thread_sum_with_fs, (void*)thread);  

   for (thread = 0; thread < thread_count; thread++) 
      pthread_join(thread_handles[thread], NULL); 
    
    for(i=0; i<thread_count; i++){
        sum+=psum[i];
    }
    #endif
   
   #ifdef GOOD
   for (thread = 0; thread < thread_count; thread++)  
      pthread_create(&thread_handles[thread], NULL,
          Thread_sum, (void*)thread);  

   for (thread = 0; thread < thread_count; thread++) 
      pthread_join(thread_handles[thread], NULL); 
   #endif
   
   sum = 4.0*sum;
   printf("With n = %lld terms,\n", n);
   printf("   Our estimate of pi = %.15f\n", sum);
   sum = Serial_pi(n);
   printf("   Single thread est  = %.15f\n", sum);
   printf("                   pi = %.15f\n", 4.0*atan(1.0));
   free(thread_handles);
   return 0;
}  /* main */

/*------------------------------------------------------------------
 *  * Function:       Thread_sum
 *   * Purpose:        Add in the terms computed by the thread running this 
 *    * In arg:         rank
 *     * Ret val:        ignored
 *      * Globals in:     n, thread_count
 *       * Global in/out:  sum 
 *        */
void* Thread_sum(void* rank) {
   long my_rank = (long) rank;
   double factor;
   long long i;
   long long my_n = n/thread_count;
   long long my_first_i = my_n*my_rank;
   long long my_last_i = my_first_i + my_n;
    
   long long j;
    //make sure the repititions do not affect the pi value, used to reset sum in every repitition 
   double prev_sum = sum;   

    for(j=REPEAT; j>0; j--){
        sum = prev_sum;
        if (my_first_i % 2 == 0)
            factor = 1.0;
        else
            factor = -1.0;

        for (i = my_first_i; i < my_last_i; i++, factor = -factor) {
            sum += factor/(2*i+1);  
        }
    }
   return NULL;
}  /* Thread_sum */

void* Thread_sum_with_fs(void* rank){
    long my_rank = (long) rank;
    double factor;
    
   long long i;
   long long my_n = n/thread_count;
   long long my_first_i = my_n*my_rank;
   long long my_last_i = my_first_i + my_n;
   long long j; 
    
    for(j=REPEAT; j>0; j--){
        if (my_first_i % 2 == 0)
            factor = 1.0;
        else
            factor = -1.0;
        psum[my_rank] = 0.0;
        for (i = my_first_i; i < my_last_i; i++, factor = -factor) {
            psum[my_rank] += factor/(2*i+1);  
        }
    }
    return NULL;
}

/*------------------------------------------------------------------
 *  * Function:   Serial_pi
 *   * Purpose:    Estimate pi using 1 thread
 *    * In arg:     n
 *     * Return val: Estimate of pi using n terms of Maclaurin series
 *      */
double Serial_pi(long long n) {
   double sum = 0.0;
   long long i;
   double factor = 1.0;

   for (i = 0; i < n; i++, factor = -factor) {
      sum += factor/(2*i+1);
   }
   return 4.0*sum;

}  /* Serial_pi */

/*------------------------------------------------------------------
 *  * Function:    Get_args
 *   * Purpose:     Get the command line args
 *    * In args:     argc, argv
 *     * Globals out: thread_count, n
 *      */
void Get_args(int argc, char* argv[]) {
   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   n = strtoll(argv[2], NULL, 10);
   if (n <= 0) Usage(argv[0]);
}  /* Get_args */

/*------------------------------------------------------------------
 *  * Function:  Usage
 *   * Purpose:   Print a message explaining how to run the program
 *    * In arg:    prog_name
 *     */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <n>\n", prog_name);
   fprintf(stderr, "   n is the number of terms and should be >= 1\n");
   fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
   exit(0);
}  /* Usage */

