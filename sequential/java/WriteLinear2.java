public class WriteLinear2 {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		final int DEFAULT_STRIDE = 31;
		final int REPEAT = Integer.parseInt(args[1]);

		int N, i, j, mask, stride;
		double value = 0;
		long sum2 = 0, startTime, endTime;

		N = Integer.parseInt(args[0]);

		double[] A = new double[N];

		value = 0;
		mask = N - 1; // N is a power of 2
		stride = DEFAULT_STRIDE;

		startTime = System.currentTimeMillis();

		for (i = 0; i < REPEAT; i++)
			for (j = 0; j < N; j++){
				sum2 += ((j+523) * 253573 ) & mask + j * stride + j;
				A[j] = value++;
			}

		endTime = System.currentTimeMillis();
		System.out.println("# Write: Linear2    : Good   : ");
		System.out.println("N=" + N + " : DataSize=" + (N * 8 / 1024)
				+ "KB : Elapsed(msec)= : " + (endTime - startTime));

	}

}
