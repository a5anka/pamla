/*********************************************************************
 *  Compilation:  javac ReadModifyWriteStride.java
 *  Execution:    java ReadModifyWriteStride N REPEAT
 *
 *  Read integers from array using a Stride algorithm
 *
 *********************************************************************/

public class ReadModifyWriteStride {

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
        int value = 1;

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
                    k = m * stride + j;
                    A[k] = A[k] + value++;
                }
            }
        }

        long endTime   = System.currentTimeMillis();

        System.out.print("ReadModifyWrite:Stride:BAD:");
        System.out.format("N=%d:DataSize=%dKB:Elapsed(msec)=%d\n",n ,dataSize, endTime - startTime);

    }
}
