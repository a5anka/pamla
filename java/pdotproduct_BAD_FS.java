public class pdotproduct_BAD_FS extends Thread {

	private static int N = 1000000;
	private static int STRIDE = 13;
	private static int REPEAT = 10;
	private static final int CACHELINE = 64; // size of cacheline is 64 bytes
	private static final int DATASIZE = 4;
	private static final int MAXTHREADS = CACHELINE / DATASIZE;

	public static int[] psum = new int[MAXTHREADS]; // partial sum computed by
													// each thread

	public static int sumtotal = 0;
	public static int numthreads;

	public static int[] v1 = new int[N];
	public static int[] v2 = new int[N];

	public static void main(String[] args) {

		int correct = 0, computed = 0;
		int i, tId[];
		Thread threads[];

		long t0 = System.currentTimeMillis();

		if (args.length != 3) {
			System.out
					.println("Usage:java psumscalar <numthreads> <N> <REPEAT> <STRIDE>");
			System.exit(0);
		}
		numthreads = Integer.parseInt(args[0]);
		N = Integer.parseInt(args[1]);
		REPEAT = Integer.parseInt(args[2]);
		STRIDE = Integer.parseInt(args[3]);

		if (numthreads > MAXTHREADS) {
			System.out.println("numthreads > MAXTHREADS " + MAXTHREADS);
			System.exit(0);
		}

		tId = new int[MAXTHREADS];
		threads = new Thread[MAXTHREADS];

		for (i = 0; i < N; i++) {
			v1[i] = 1;
			v2[i] = 2; // more expensive options: (i+1) % 3 or random() % 3;

		}
		correct = 2 * N;

		long t1 = System.currentTimeMillis();

		for (i = 0; i < numthreads; i++) {
			tId[i] = i;
			psum[i] = 0;

			pdotproduct_BAD_FS t = new pdotproduct_BAD_FS();
			threads[i] = t;
			t.setName("" + i);
			t.start();
		}

		for (int j = 0; j < numthreads; j++) {
			try {
				threads[j].join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		long t2 = System.currentTimeMillis();

		for (i = 0; i < numthreads; i++)
			computed += psum[i];
		correct = REPEAT * correct;

		if (computed != correct || (correct != sumtotal))
			System.out.println("# Error! : correct=" + correct + ", computed="
					+ computed + ", sumtotal=" + sumtotal);

		long t3 = System.currentTimeMillis();

		long comp_time = t2 - t1;
		long total_time = t3 - t0;

		if (numthreads == 1) {
			System.out.println("# PDotProduct: GOOD : N=" + N + " : Threads="
					+ numthreads + " : CompTime(ms)=" + comp_time
					+ " : CompTime/TotalTime=" + (comp_time * 100 / total_time)
					+ "%");
		} else
			System.out.println("# PDotProduct: Bad-FS : N=" + N + " : Threads="
					+ numthreads + " : CompTime(ms)=" + comp_time
					+ " : CompTime/TotalTime=" + (comp_time * 100 / total_time)
					+ "%");

	}

	public void run() {

		int tId = Integer.parseInt(this.getName());
		int start = tId * N / numthreads;
		int end = (tId + 1) * N / numthreads;
		int localSum[] = new int[MAXTHREADS];
		int i, n = 0, k = tId;

		for (int j = 0; j < REPEAT; j++) {

			for (i = start; i < end; i++) {
				n += (start + (i * STRIDE + k)) % 2;
				psum[tId] += v1[i] * v2[i]; // causes false sharing among
											// threads
			}

		}

		localSum[tId] = n;
		sumtotal += psum[tId];
	}

}
