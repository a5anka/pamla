//	read.c

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
                                        { perror("gettime( ):"); exit(EXIT_FAILURE); }
#define		DEFAULT_STRIDE	31
#ifdef          LINEAR1
#define         MSG1		"Read:Linear1:Good:"
#endif
#ifdef          LINEAR2
#define         MSG1		"Read:Linear2:Good:"
#endif
#ifdef          RANDOM1
#define         MSG1		"Read:Random1:Bad-MA:"
#endif
#ifdef          RANDOM2
#define         MSG1		"Read:Random2:Bad-MA:"
#endif
#ifdef          STRIDE
#define         MSG1		"Read:Stride="
#endif
#define         MSG2		"N=%d:DataSize=%dKB:Elapsed(msec)=%.2f:"

float elapsedTime(struct timespec *, struct timespec *, long *, long *);	// proto type

TYPE A[N]; //, B[N];
int main (int argc, char * argv[])
{
    int                 i, j, k, m, n, mask, stride;
    TYPE                sum=0, result = 0;
    int64_t		sum2=0;
    struct              timespec t1, t2;
    unsigned long       sec, nsec;
    float               elapsed_msec;

    for (i=0; i < N; i++) A[i] = i * 1.0;       // init time not counted

    GET_TIME(t1);
    result = sum = 0;
    mask = N -1 ;	// N is a power of 2
    stride = DEFAULT_STRIDE;
#ifdef LINEAR1
    for (i = 0; i < REPEAT; i++)
        for (j = 0; j < N; j++){
            sum2 += ((j+523) * 253573 ) & mask;
            sum += A[j];
        }
#endif
#ifdef LINEAR2
    for (i = 0; i < REPEAT; i++)
        for (j = 0; j < N; j++){
            sum2 += ((j+523) * 253573 ) & mask + j * stride + j;
            sum += A[j];
                }
#endif
#ifdef RANDOM1
    for (i = 0; i < REPEAT; i++)
        for (j = 0; j < N; j++){
            k = ((j+523) * 253573 ) & mask;
            sum2 += k;
            sum += A[k];
        }
#endif
#ifdef RANDOM2
    for (i = 0; i < REPEAT; i++)
                for (j = 0; j < N; j++){
                    k = ((j+523) * 253573 ) & mask;
                    sum2 += k + j * stride + j;
                    sum += A[k];
                }
#endif
#ifdef STRIDE
    stride = STRIDE;
    for (i = 0; i < REPEAT; i++)
        for (j = 0; j < STRIDE; j++)
            for (m=0; m < N/STRIDE; m++) {
                sum2 += ((m+523) * 253573 ) & mask;
                sum += A[m * STRIDE + j];
            }
#endif

    result = sum + sum2;
    GET_TIME(t2);
    elapsed_msec = elapsedTime(&t1, &t2, &sec, &nsec);
    printf(MSG1);
#ifdef STRIDE
    printf("%d:Bad-MA:", STRIDE);
#endif
    //printf(MSG2, N, elapsed_msec, (int) sec, (int) nsec);
    printf(MSG2, N, N*sizeof(TYPE)/1024, elapsed_msec);
    //#ifdef WARMUP
    //printf(" -- WARM\n");
    //#else
    printf("\n");
    //#endif

    return 0;
}
//-----------------------------------------------
float elapsedTime(struct timespec *begin, struct timespec *end, long *sec, long *nsec)
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
