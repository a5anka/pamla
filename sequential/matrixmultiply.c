//	matrixmultiply.c
//	Compile example: gcc -DN=1024 -DIKJ -DTYPE=double -DCHECK matrixmultiply.c -lrt

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define GET_TIME(x);	if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
                                        { perror("gettime( ):"); exit(EXIT_FAILURE); }
#define ERROR(i,j);	printf("--Error: C[%d][%d]=%.1f\n", (i), (j), C[(i)][(j)]); \
                                        fflush(stdout); exit(EXIT_FAILURE);
#ifdef          IJK
#define         MSG1		"# MatMult: ijk :         :  "
#endif
#ifdef          IKJ
#define         MSG1		"# MatMult: ikj : Good    :  "
#endif
#ifdef          JIK
#define         MSG1		"# MatMult: jik :         :  "
#endif
#ifdef          JKI
#define         MSG1		"# MatMult: jki : Bad-MA  :  "
#endif
#ifdef          KIJ
#define         MSG1		"# MatMult: kij : Good    :  "
#endif
#ifdef          KJI
#define         MSG1		"# MatMult: kji : Bad-MA  :  "
#endif
#ifdef          TRANSPOSE
#define         MSG1		"# MatMult: B-Tran : Good :  "
#endif
#ifdef          BLOCK
#ifdef          TJK__IJK
#define         MSG1		"# MatMult: tjk--ijk Block = "
#endif
#ifdef          TJKI_IJK
#define         MSG1		"# MatMult: tjki-ijk Block = "
#endif
#ifdef          JKI_IKJ
#define         MSG1		"# MatMult: jki-ikj Block  = "
#endif
#ifdef          JK__IKJ
#define         MSG1		"# MatMult: jk--ikj Block  = "
#endif
#ifdef          KJ__IJK
#define         MSG1		"# MatMult: kj--ijk Block  = "
#endif
#ifdef          KJ__IKJ
#define         MSG1		"# MatMult: kj--ikj Block  = "
#endif
#ifdef          KJI_IKJ
#define         MSG1		"# MatMult: kji-ikj Block  = "
#endif
#endif
//#define		MSG2		"N = %d : Elapsed time: %.3f msec (<%d> sec + <%d> nsec)\n"
#define		MSG2		"N= %4d : DataSize= %6d KB : Elapsed-Time(msec)=%10.3f "

float elapsedTime(struct timespec *, struct timespec *, long *, long *);	// proto type

TYPE A[N][N], B[N][N], C[N][N];	// ifdef TRANSPOSE, we assume B has been transposed

int main (int argc, char * argv[])
{
    int                                 i, j, k, ii, jj, kk;
    TYPE                                sum=0, r = 0, result;
    struct timespec             t1, t2;
    unsigned long               sec, nsec;
    float                       elapsed_msec;

    // initialize
    for (i=0; i < N; i++)
        for (j=0; j < N; j++) {
            A[i][j] = B[i][j] = (TYPE)1.0; // A, B symmetric here, so Tranpose(B) = B
            C[i][j] = 0;
        }
    // -- end initialize

    // 6 versions of matrix multiply: each computes the same, performs O(n^3) total operations
    // and identical # of additions, mults; each n^2 elems in A, B read n times, each n^2 elem in C
    // computed by adding n values

    // Analysis of inner loops (numbers are per inner loop interation)
    // assuming:	- cache line (block) size = 64 B, data (double) word size = 8 B, N is large
    //			- A matrix row will not fit in L1 cache; all scalars in registers
    // Version (class)	Loads	Stores	A_miss	B_miss	C_miss	Total_miss
    // ---------------	-----	------	-------	------	------	----------
    // ikj, kij (CB)	2	1		0		1/8		1/8		0.25
    // ijk, jik (AB)	2	0		1/8		1		0		1.125
    // jki, kji (CA)	2	1		1		0		1		2
    // Transpose		2	0		1/8		1/8		0		0.25

    GET_TIME(t1);
#ifdef IKJ		// class BC: ikj
    for (i = 0; i < N; i++)
        for (k = 0; k < N; k++) {
            r = A[i][k];
                        for (j = 0; j < N ; j++)
                            C[i][j] += r * B[k][j];
        }
#endif
#ifdef KIJ		// class BC: kij
    for (k = 0; k < N; k++)
        for (i = 0; i < N; i++) {
            r = A[i][k];
            for (j=0; j < N ; j++)
                C[i][j] +=  r * B[k][j] ;
                }
#endif
#ifdef IJK		// class AB: ijk
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++){
                        sum = 0;
                        for (k=0; k < N; k++)
                            sum += A[i][k] * B[k][j];
                        C[i][j] = sum;
        }
        #endif
#ifdef JIK		// class AB: jik
    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++) {
            sum = 0;
            for (k=0; k < N ; k++)
                sum += A[i][k] * B[k][j] ;
            C[i][j] = sum;
        }
#endif
#ifdef JKI		// class CA: jki
    for (j = 0; j < N; j++)
        for (k = 0; k < N; k++) {
            r = B[k][j];
            for (i=0; i < N ; i++)
                C[i][j] += A[i][k] * r ;
        }
#endif
#ifdef KJI		// class CA: kji
    for (k = 0; k < N; k++)
        for (j = 0; j < N; j++) {
            r = B[k][j];
            for (i=0; i < N ; i++)
                C[i][j] += A[i][k] * r ;
        }
#endif
#ifdef TRANSPOSE
    // multiply (A)(Tranpose(B)) ijk order to get C; here B = Tranpose(B)
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++){
            sum = 0;
            for (k=0; k < N; k++)
                sum += A[i][k] * B[j][k];	// B[j][k] order due to transpose
            C[i][j] = sum;
            }
#endif
    //------ begin BLOCK approach
#ifdef BLOCK		// assumes N is a multiple of BLOCK
#ifdef TJK__IJK		// version: AB: tjk--ijk
    for (jj = 0; jj < N; jj += BLOCK) {
        for (kk=0; kk < N; kk += BLOCK) {
            for (i = 0; i < N; i++) {
                for (j = jj; j < jj + BLOCK; j++) {
                    sum = C[i][j];
                    for (k = kk; k < kk + BLOCK ; k++)
                        sum += A[i][k]* B[j][k];	// transposed
                    C[i][j] = sum;
                }
            }
        }
    }
#endif
#ifdef TJKI_IJK		// version: AB: tjki-ijk
    for (jj = 0; jj < N; jj += BLOCK) {
        for (kk=0; kk < N; kk += BLOCK) {
            for (ii = 0; ii < N; ii += BLOCK) {
                for (i = ii; i < ii + BLOCK; i++) {
                    for (j = jj; j < jj + BLOCK; j++) {
                        sum = C[i][j];
                        for (k = kk; k < kk + BLOCK ; k++)
                            sum += A[i][k]* B[j][k];	// transposed
                        C[i][j] = sum;
                    }
                }
            }
        }
    }
#endif
#ifdef JK__IKJ		// version: CB: jk--ikj
    for (jj = 0; jj < N; jj += BLOCK) {
        for (kk=0; kk < N; kk += BLOCK) {
            for (i = 0; i < N; i++) {
                for (k = kk; k < kk + BLOCK; k++) {
                    r = A[i][k];
                    for (j = jj; j < jj + BLOCK ; j++)
                        C[i][j] += r * B[k][j];
                    }
            }
        }
    }
#endif
#ifdef JKI_IKJ		// version: CB: jki-ikj
    for (jj = 0; jj < N; jj += BLOCK) {
        for (kk=0; kk < N; kk += BLOCK) {
            for (ii = 0; ii < N; ii += BLOCK) {
                for (i = ii; i < ii + BLOCK ; i++) {
                    for (k = kk; k < kk + BLOCK; k++) {
                        r = A[i][k];
                        for (j = jj; j < jj + BLOCK ; j++)
                            C[i][j] += r * B[k][j];
                    }
                }
            }
        }
    }
#endif
#ifdef KJ__IJK		// version: AB: kj--ijk	(*** generally worst among BLOCK methods)
    for (kk=0; kk < N; kk += BLOCK) {
        for (jj = 0; jj < N; jj += BLOCK) {
            for (i = 0; i < N; i++) {
                for (j = jj; j < jj + BLOCK; j++) {
                    sum = C[i][j];		// try sum = 0;
                    for (k=kk ; k < kk + BLOCK; k++)
                        sum += A[i][k] * B[k][j];
                    C[i][j] = sum;
                }
            }
        }
    }
#endif
#ifdef KJ__IKJ		// version: CB: kj--ikj
    for (kk=0; kk < N; kk += BLOCK) {
        for (jj = 0; jj < N; jj += BLOCK) {
            for (i = 0; i < N; i++) {
                for (k = kk; k < kk + BLOCK; k++) {
                    r = A[i][k];
                    for (j = jj; j < jj + BLOCK ; j++)
                        C[i][j] += r * B[k][j];
                }
            }
        }
    }
#endif
#ifdef KJI_IKJ		// version: CB: kji-ikj
    for (kk=0; kk < N; kk += BLOCK) {
        for (jj = 0; jj < N; jj += BLOCK) {
            for (ii = 0; ii < N; ii += BLOCK) {
                for (i = ii; i < ii + BLOCK; i++) {
                    for (k = kk; k < kk + BLOCK; k++) {
                        r = A[i][k];
                        for (j = jj; j < jj + BLOCK ; j++)
                            C[i][j] += r * B[k][j];
                    }
                }
            }
        }
    }
#endif
#endif	//------ end BLOCK approach
    result = sum;
    GET_TIME(t2);
    elapsed_msec = elapsedTime(&t1, &t2, &sec, &nsec);
    printf(MSG1);
#ifdef BLOCK
    printf("%4d : ", BLOCK);
#endif
    printf(MSG2, N, 3*N*N*sizeof(TYPE)/1024, elapsed_msec);	// 3 arrays, each N*N size
#ifdef CHECK		// check resulls
    for (i=0; i < N; i++)
        for (j=0; j < N; j++) {
            if (C[i][j] != (TYPE) N) {
                ERROR(i,j);
            }
        }
#endif
    printf("\n");

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
