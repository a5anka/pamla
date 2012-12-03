
public class WriteStride {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
	
		final int DEFAULT_STRIDE = 31;
		final int REPEAT = Integer.parseInt(args[1]);
		final int STRIDE = Integer.parseInt(args[2]);

		int N, i, j, m, mask, stride;
		double value = 0;
		long sum2 = 0, startTime, endTime;

		N = Integer.parseInt(args[0]);

		double[] A = new double[N];

		value = 0;
		mask = N - 1; // N is a power of 2
		stride = DEFAULT_STRIDE;

		startTime = System.currentTimeMillis();

		stride = STRIDE;
		for (i = 0; i < REPEAT; i++)
			for (j = 0; j < STRIDE; j++)
				for (m=0; m < N/STRIDE; m++) {
					sum2 += ((m+523) * 253573 ) & mask;
					A[m * STRIDE + j] = value++;
			}

		endTime = System.currentTimeMillis();
		System.out.print("# Write: Stride=");
		System.out.format("%4d : Bad-MA : ", STRIDE);
		System.out.println("N=" + N + " : DataSize=" + (N * 8 / 1024)
				+ "KB : Elapsed(msec)= : " + (endTime - startTime));

	}

}
