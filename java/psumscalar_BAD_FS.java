public class psumscalar_BAD_FS extends Thread {

	private static int N = 100000000;
	private static int REPEAT = 10;
	private static final int CACHELINE = 64; // size of cacheline is 64 bytes
	private static final int DATASIZE = 4;
	private static final int MAXTHREADS = CACHELINE / DATASIZE;

	public static int[] psum = new int[MAXTHREADS]; // partial sum computed by
													// each thread

	public static int sumtotal = 0;
	public static int numthreads;

	public static void main(String[] args) {

		int result = 0;
		int i, myid[];
		Thread threads[];

		long t1 = System.currentTimeMillis();

		if (args.length != 3) {
			System.out.println("Usage:java psumscalar <numthreads> <N> <REPEAT>");
			System.exit(0);
		}
		numthreads = Integer.parseInt(args[0]);
		N = Integer.parseInt(args[1]);
		REPEAT = Integer.parseInt(args[2]);

		if (numthreads > MAXTHREADS) {
			System.out.println("numthreads > MAXTHREADS " + MAXTHREADS);
			System.exit(0);
		}

		myid = new int[MAXTHREADS];
		threads = new Thread[MAXTHREADS];

		long t2 = System.currentTimeMillis();

		for (i = 0; i < numthreads; i++) {
			myid[i] = i;
			psum[i] = 0;

			psumscalar_BAD_FS t = new psumscalar_BAD_FS();
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

		long t3 = System.currentTimeMillis();

		for (int j = 0; j < numthreads; j++) {
			result += psum[j];
		}

		if (result != sumtotal)
			System.out.println("# Error! : result=" + result + ", sumtotal="
					+ sumtotal);

		long comp_time = t3 - t2;
		long total_time = t3 - t1;

		if (numthreads == 1) {
			System.out
					.println("# PSumScalar: GOOD : N=" + N + " : Threads="
							+ numthreads + " : CompTime(ms)=" + comp_time
							+ " : CompTime/TotalTime="
							+ (comp_time / total_time) * 100);
		}
		else
		System.out.println("# PSumScalar: Bad-FS : N=" + N + " : Threads="
				+ numthreads + " : CompTime(ms)=" + comp_time
				+ " : CompTime/TotalTime=" + (comp_time / total_time) * 100);

	}

	public void run() {
		int tId = Integer.parseInt(this.getName());
		long start = tId * N / numthreads;
		long end = (tId + 1) * N / numthreads;
		int localSum[] = new int[MAXTHREADS];

		for (int j = 0; j < REPEAT; j++) {
			for (int i = (int) start; i < (int) end; i++) {
				psum[tId] += i % 3; // causes false sharing among threads
			}
		}

		sumtotal += psum[tId];
	}

}
