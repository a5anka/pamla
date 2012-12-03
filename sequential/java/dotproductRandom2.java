public class dotproductRandom2 {

	static int N,REPEAT;
		static double[] A;
		static double[] B;
		
		final static int DEFAULT_STRIDE=31;
		
		public static void main(String[] args) {
			long starttime,endtime;
			/*
			try {
				Thread.sleep(3000);
			} catch (InterruptedException e) {
			}
			*/
			assert args.length==2;
			N=Integer.parseInt(args[0]);
			REPEAT=Integer.parseInt(args[1]);
			A= new double[N];
			B= new double[N];
			
			int i, j, k, m, mask, stride;
		    double sum=0.0, result = 0.0;
		    int	sum2=0;

		    for (i=0; i < N; i++) A[i] = B[i] = i* 1.0;

		    result = sum = 0;
		    mask = N -1 ;	// N is a power of 2
		    stride = DEFAULT_STRIDE;
		    
		    //RANDOM2
		    starttime=System.currentTimeMillis();
		    for (i = 0; i < REPEAT; i++)
		        for (j = 0; j < N; j++){
		            k = ((j+523) * 253573 ) & mask;
		            sum2 += k + j * stride + j;
		            sum += A[k] * B[k];
		        }
		    endtime=System.currentTimeMillis();
		    System.out.format("N=%d:DataSize=%dKB:Elapsed(msec)=%d\n",N,((8*N)/1024)*2,(endtime-starttime));
		}
	}