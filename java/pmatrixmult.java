class pmatrixmult extends Thread {

	private static final int N = 1000;
	private static final int STRIDE = 13;
	private static final int CACHELINE = 64; // size of cacheline is 64 bytes
	private static final int DATASIZE = 4;
	private static final int MAXTHREADS = CACHELINE / DATASIZE;

	private static final boolean GOOD = true;
	private static final boolean BAD_MA = false;
	private static final boolean BAD_FS = false ;
	
	private static final boolean CHECK = true;

	// global variables
	public static int localSumtotal = 0;
	public static int numthreads;
	public static int[] plocalSum = new int[MAXTHREADS]; // partial localSum computed by
													// each thread
	public static int[][] A = new int[N][N];
	public static int[][] B = new int[N][N];
	public static int[][] C = new int[N][N];
	public static int[][] Cp = new int[N][N];

	public static void main(String[] args) {

		int result = 0;
		int i, tId[];
		Thread threads[];

		long t0 = System.currentTimeMillis();
		
		if (args.length != 1) {
			System.out.println("Usage:java pmatrixmult <numthreads>");
			System.exit(0);
		}
		numthreads = Integer.parseInt(args[0]);
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

			pmatrixmult t = new pmatrixmult();
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

		if(CHECK){
			int error = 0;
			for ( i=0; i < N; i++) {
				for (int j=0; j < N; j++) {
					Cp[i][j]= 0;
					for (int k=0; k < N; k++)
						Cp[i][j] += A[i][k]*B[k][j];
					if (Cp[i][j] != C[i][j])
						error++;
				}
			}
			if (error>0)
				System.out.println("# Error! : count = %d *************"+error);
		}

		long t3 = System.currentTimeMillis();
		
		long comp_time = t2 - t1;
		long total_time = t3 - t0;

		if (BAD_FS) {
			if (numthreads == 1) {
				System.out.println("# PMatrixMult: GOOD : N=" + N
						+ " : Threads=" + numthreads + " : CompTime(ms)="
						+ comp_time + " : CompTime/TotalTime="
						+ (comp_time*100 / total_time)+"%");
			}
		}

		if (BAD_FS) {
			System.out
					.println("# PMatrixMult: Bad-FS : N=" + N + " : Threads="
							+ numthreads + " : CompTime(ms)=" + comp_time
							+ " : CompTime/TotalTime="
							+ (comp_time *100/ total_time)+"%");
		}

		if (GOOD) {
			System.out
					.println("# PMatrixMult: GOOD : N=" + N + " : Threads="
							+ numthreads + " : CompTime(ms)=" + comp_time
							+ " : CompTime/TotalTime="
							+ (comp_time *100 / total_time)+"%");
		}
		
		if (BAD_MA) {
			System.out
					.println("# PMatrixMult: BAD_MA : N=" + N + " : Threads="
							+ numthreads + " : CompTime(ms)=" + comp_time
							+ " : CompTime/TotalTime="
							+ (comp_time *100 / total_time)+"%");
		}

	}

	public void run() {
		int tId = Integer.parseInt(this.getName());
		int start = tId * N / numthreads;
		int end = (tId + 1) * N / numthreads;
		int localSum[] = new int[MAXTHREADS];

		for (int j = start; j < end; j++) {
			for (int i = 0; i < N; i++) {
				if (GOOD) {
					localSum[tId] = 0;
					for (int k = 0; k < N; k++) {
						localSum[tId] += A[i][k] * B[k][j];
					}
					C[i][j] = localSum[tId];
				}

				else if (BAD_FS) {
					plocalSum[tId] = 0;
					for (int k = 0; k < N; k++) {
						plocalSum[tId] += A[i][k] * B[k][j]; // causes false sharing
														// in caches
					}
					C[i][j] = plocalSum[tId];
				}

				else if (BAD_MA) {
					localSum[tId] = 0;
					for (int n = 0; n < STRIDE; n++) {
						for (int k = 0; k < N / STRIDE; k++) {
							localSum[tId] += A[i][k * STRIDE + n]
									* B[k * STRIDE + n][j];
						}
					}
					if ((N / STRIDE) * STRIDE != N) {
						for (int k = (N / STRIDE) * STRIDE; k < N; k++)
							localSum[tId] += A[i][k] * B[k][j];
					}
					C[i][j] = localSum[tId]; // can be +=
				}

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
