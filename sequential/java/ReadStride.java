/*********************************************************************
 *  Compilation:  javac ReadStride.java
 *  Execution:    java ReadStride N REPEAT
 *
 *  Read integers from array using a Stride algorithm
 *
 *********************************************************************/

public class ReadStride {

    public static double[] A;
    public final static int DOUBLE_SIZE = 8;
    public final static int DEFAULT_STRIDE = 31;

    public static void main(String[] args) {
        assert args.length == 3;

        int n = Integer.parseInt(args[0]), k;
        int repeat = Integer.parseInt(args[1]);
        int stride =Integer.parseInt(args[2]);
        int mask = n - 1;
        long sum2 = 0;
        double sum = 0;
        long dataSize = n * DOUBLE_SIZE / 1024;

        A = new double[n];

        for (int i=0; i < n; i++)
            {
                A[i] = i * 1.0;
            }

        long startTime = System.currentTimeMillis();

        for (int i = 0; i < repeat; i++) {
            for (int j = 0; j < stride; j++) {
                for (int m=0; m < n/stride; m++) {
                    sum2 += ((m+523) * 253573 ) & mask;
                    sum += A[m * stride + j];
                }
            }
        }

        long endTime   = System.currentTimeMillis();

        System.out.format("Read:Stride:BAD:N=%d:DataSize=%dKB:Elapsed(msec)=%d\n",n ,dataSize, endTime - startTime);

    }
}
