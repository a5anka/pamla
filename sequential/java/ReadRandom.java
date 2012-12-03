/*********************************************************************
 *  Compilation:  javac ReadRandom.java
 *  Execution:    java ReadRandom N REPEAT
 *
 *  Reads integres from array using a random algorithm
 *
 *********************************************************************/

public class ReadRandom {

    public static double[] A;
    public final static int DOUBLE_SIZE = 8;
    public final static int DEFAULT_STRIDE = 31;

    public static void main(String[] args) {
        assert args.length == 2;

        int n = Integer.parseInt(args[0]), k;
        int stride = DEFAULT_STRIDE;
        int repeat = Integer.parseInt(args[1]);
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
            for (int j = 0; j < n; j++){
                k = ((j+523) * 253573 ) & mask;
                sum2 += k + j * stride + j;
                sum += A[k];
            }
        }

        long endTime   = System.currentTimeMillis();

        System.out.format("Read:Random:BAD:N=%d:DataSize=%dKB:Elapsed(msec)=%d\n",n ,dataSize, endTime - startTime);

    }
}
