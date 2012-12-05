
public class WriteRandom2 {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		
		final int DEFAULT_STRIDE = 31;
		final int REPEAT = Integer.parseInt(args[1]);

		int N, i, j, k, mask, stride;
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
				k = ((j+523) * 253573 ) & mask;
				sum2 += k + j * stride + j;
				A[k] = value++;
			}

		endTime = System.currentTimeMillis();
		System.out.print("Write:Random2:Bad-MA:");
		System.out.println("N=" + N + ":DataSize=" + (N * 8 / 1024)
				+ "KB:Elapsed(msec)=:" + (endTime - startTime));

	}

}
