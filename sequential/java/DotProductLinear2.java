
public class DotProductLinear2 {

	/**
	 * @param args
	 */
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
			// TODO Auto-generated catch block
			//e.printStackTrace();
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
	    
	    //LINEAR2
	    starttime=System.currentTimeMillis();
	    for (i = 0; i < REPEAT; i++)
	        for (j = 0; j < N; j++){
	            sum2 += ((j+523) * 253573 ) & mask + j * stride + j;
	            sum += A[j] * B[j];
	        }
	    endtime=System.currentTimeMillis();
	    System.out.format("DotProduct:Linear2:GOOD:N=%d:DataSize=%dKB:Elapsed(msec)=%d\n",N,((8*N)/1024)*2,(endtime-starttime));
	}
}
