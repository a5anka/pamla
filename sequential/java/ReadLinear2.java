/*********************************************************************
 *  Compilation:  javac ReadLinear2.java
 *  Execution:    java ReadLinear2 N REPEAT
 *
 *  Read integers from array using a linear algorithm
 *
 *********************************************************************/

public class ReadLinear2 {

    public static double[] A;
    public final static int DOUBLE_SIZE = 8;

    public static void main(String[] args) {
        assert args.length == 2;

        int n = Integer.parseInt(args[0]);
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
            for (int j = 0; j < n; j++)
                {
                    sum2 += ((j+523) * 253573 ) & mask;
                    sum += A[j];
                }
        }

        long endTime   = System.currentTimeMillis();

        System.out.format("Read:Linear2:Good:N=%d:DataSize=%dKB:Elapsed(msec)=%d\n",n ,dataSize, endTime - startTime);

    }
}
