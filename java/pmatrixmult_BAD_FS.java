public class pmatrixmult_BAD_FS extends Thread {

	private static int N = 1000;
	private static int REPEAT = 10;
	private static int STRIDE = 13;
	private static final int CACHELINE = 64; // size of cacheline is 64 bytes
	private static final int DATASIZE = 4;
	private static final int MAXTHREADS = CACHELINE / DATASIZE;

	private static final boolean CHECK = true;

	// global variables
	public static int localSumtotal = 0;
	public static int numthreads;
	public static int[] plocalSum = new int[MAXTHREADS]; // partial localSum
															// computed by
	// each thread
	public static int[][] A;
	public static int[][] B;
	public static int[][] C;
	public static int[][] Cp;

	public static void main(String[] args) {

		int result = 0;
		int i, tId[];
		Thread threads[];

		long t0 = System.currentTimeMillis();

		if (args.length != 4) {
			System.out
					.println("Usage:java pmatrixmult <numthreads> <N> <REPEAT> <STRIDE>");
			System.exit(0);
		}
		numthreads = Integer.parseInt(args[0]);
		N = Integer.parseInt(args[1]);
		REPEAT = Integer.parseInt(args[2]);
		STRIDE = Integer.parseInt(args[3]);

		A = new int[N][N];
		B = new int[N][N];
		C = new int[N][N];
		Cp = new int[N][N];
		
		init_matrix(A, 1);
		init_matrix(B, 2);

		if (numthreads > MAXTHREADS) {
			System.out.println("numthreads > MAXTHREADS " + MAXTHREADS);
			System.exit(0);
		}

		tId = new int[MAXTHREADS];
		threads = new Thread[MAXTHREADS];

		long t1 = System.currentTimeMillis();

		for (i = 0; i < numthreads; i++) {
			tId[i] = i;
			plocalSum[i] = 0;

			pmatrixmult_BAD_FS t = new pmatrixmult_BAD_FS();
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

		if (CHECK) {
			int error = 0;
			for (i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					Cp[i][j] = 0;
					for (int k = 0; k < N; k++)
						Cp[i][j] += A[i][k] * B[k][j];
					if (Cp[i][j] != C[i][j])
						error++;
				}
			}
			if (error > 0)
				System.out.println("# Error! : count = %d *************"
						+ error);
		}

		long t3 = System.currentTimeMillis();

		long comp_time = t2 - t1;
		long total_time = t3 - t0;

		if (numthreads == 1) {
			System.out.println("# PMatrixMult: GOOD : N=" + N + " : Threads="
					+ numthreads + " : CompTime(ms)=" + comp_time
					+ " : CompTime/TotalTime=" + (comp_time * 100 / total_time)
					+ "%");
		} else
			System.out.println("# PMatrixMult: Bad-FS : N=" + N + " : Threads="
					+ numthreads + " : CompTime(ms)=" + comp_time
					+ " : CompTime/TotalTime=" + (comp_time * 100 / total_time)
					+ "%");

	}

	public void run() {
		int tId = Integer.parseInt(this.getName());
		int start = tId * N / numthreads;
		int end = (tId + 1) * N / numthreads;
		int localSum[] = new int[MAXTHREADS];

		for (int j = start; j < end; j++) {
			for (int i = 0; i < N; i++) {

				plocalSum[tId] = 0;
				for (int k = 0; k < N; k++) {
					plocalSum[tId] += A[i][k] * B[k][j]; // causes false
															// sharing
					// in caches
				}
				C[i][j] = plocalSum[tId];

			}
		}
	}

	static void init_matrix(int[][] a2, int v) {
		int i, j;
		v = v + 1;
		for (i = 0; i < N; i++)
			for (j = 0; j < N; j++)
				a2[i][j] = j % v; // random() % v is more expensive

	}

}
