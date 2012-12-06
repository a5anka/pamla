/*********************************************************************
 *  Compilation:  javac ReadModifyWriteRandom2.java
 *  Execution:    java ReadModifyWriteRandom2 N REPEAT
 *
 *  ReadModifyWrite integers from array using a random algorithm
 *
 *********************************************************************/

public class ReadModifyWriteRandom2 {

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
        int value = 1;

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
                A[k] = A[k] + value++;
            }
        }

        long endTime   = System.currentTimeMillis();

        System.out.print("ReadModifyWrite:Random2:BAD:");
        System.out.format("N=%d:DataSize=%dKB:Elapsed(msec)=%d\n",n ,dataSize, endTime - startTime);

    }
}
