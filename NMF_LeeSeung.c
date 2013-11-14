#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef ATLAS
#include <cblas.h>
#endif
#ifdef GPU
#include <cuda.h>
#include "cublas.h"
#include "kernels.h"
#endif
#include <sys/times.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#undef DEBUG
#define verbose 0

#ifndef FATLAS
#ifdef REAL
	#define real float
	#define cblas_rgemm cblas_sgemm
	#define cblas_rdot cblas_sdot
	#define cblas_rcopy cblas_scopy
	#define rmax(a,b) ( ( (a) > (b) )? (a) : (b) )
	#define rsqrt sqrtf
#else
	#define real double
	#define cblas_rgemm cblas_dgemm
	#define cblas_rdot cblas_ddot
	#define cblas_rcopy cblas_dcopy
	#define rmax fmax
	#define rsqrt sqrt
#endif
#else
#ifdef REAL
	#define real float
	#define rgemm sgemm_
	#define rdot sdot_
	#define rcopy scopy_
	#define rmax(a,b) ( ( (a) > (b) )? (a) : (b) )
	#define rsqrt sqrtf_
#else
	#define real double
	#define rgemm dgemm_
	#define rdot ddot_
	#define rcopy dcopy_
	#define rmax fmax_
	#define rsqrt sqrt_
#endif

#endif

#ifdef FATLAS
void  sgemm_(char *TA, char *TB, int *M, int *N, int*K, float *ALPHA, float *A, int *LDA, float *B, int *LDB, float *BETA, float *C, int *LDC);
void  dgemm_(char *TA, char *TB, int *M, int *N, int*K, double *ALPHA, double *A, int *LDA, double *B, int *LDB, double *BETA, double *C, int *LDC);
#endif




/* Number of iterations before testing convergence (can be adjusted) */
#define NITER_TEST_CONV 10

/* Spacing of floating point numbers. */
real eps=2.2204e-16;

void printMATRIX(real **m, int I, int J);

double gettime()
{
	double final_time;
	struct timeval tv1;
	
	gettimeofday(&tv1, (struct timezone*)0);
	final_time = (tv1.tv_usec + (tv1.tv_sec)*1000000ULL);

	return(final_time);
}

real *get_memory1D( int nx )
{ 
	int i;
	real *buffer;	

	if( (buffer=(real *)malloc(nx*sizeof(real)))== NULL )
	{
		fprintf( stderr, "ERROR in memory allocation\n" );
		return( NULL );
	}

	for( i=0; i<nx; i++ )
	{
		buffer[i] = (real)(i*10);
	}

	return( buffer );
}


void delete_memory1D( real *buffer )
{ 
	free(buffer);
}

unsigned char *get_memory1D_uchar( int nx )
{ 
	int i;
	unsigned char *buffer;	

	if( (buffer=(unsigned char *)malloc(nx*sizeof(int)))== NULL )
	{
		fprintf( stderr, "ERROR in memory allocation\n" );
		return( NULL );
	}

	for( i=0; i<nx; i++ )
	{
		buffer[i] = (int)(0);
	}

	return( buffer );
}


void delete_memory1D_uchar( unsigned char *buffer )
{ 
	free(buffer);
}

real **get_memory2D( int nx, int ny )
{ 
	int i,j;
	real **buffer;
	real *tmp_Pinned;

	if( (buffer=(real **)malloc(nx*sizeof(real *)))== NULL )
	{
		fprintf( stderr, "ERROR in memory allocation\n" );
		return( NULL );
	}

#ifdef pinned_memory
	cudaError_t status = cudaMallocHost((void**)&(tmp_Pinned), nx*ny*sizeof(real));
	if (status != cudaSuccess)
	{
		fprintf( stderr, "ERROR in pinned memory allocation\n" );
		free( buffer );
		return( NULL );
	}

#else
	if( (buffer[0]=(real *)malloc(nx*ny*sizeof(real)))==NULL )
	{
		fprintf( stderr, "ERROR in memory allocation\n" );
		free( buffer );
		return( NULL );
	}
#endif
#ifdef pinned_memory
	buffer[0] = tmp_Pinned;
#endif
	for( i=1; i<nx; i++ )
	{
		buffer[i] = buffer[i-1] + ny;
	}

	for( i=0; i<nx; i++ )
		for( j=0; j<ny; j++ )
		{
			buffer[i][j] = (real)(i*100+j);
		}

	return( buffer );
}

void delete_memory2D( real **buffer )
{ 
#ifdef pinned_memory
	cudaFreeHost(buffer[0]);
#else
	free(buffer[0]);
#endif
	free(buffer);
}

real **get_memory2D_renew( int nx, int ny, real *mem )
{ 
	int i,j;
	real **buffer;	

	if( (buffer=(real **)malloc(nx*sizeof(real *)))== NULL )
	{
		fprintf( stderr, "ERROR in memory allocation\n" );
		return( NULL );
	}


	for( i=0; i<nx; i++ )
	{
		buffer[i] = &(mem[i*ny]);
	}


	return( buffer );
}

void matrix_copy1D_uchar( unsigned char *in, unsigned char *out, int nx )
{ 
	int i;

	for (i=0; i<nx; i++)
		out[i] = in[i];
}

void matrix_copy2D(real **in, real **out, int nx, int ny)
{
	int i,j;
	
	for (i=0; i<nx; i++)
		for(j=0;j<ny;j++)
			out[i][j] = in[i][j];
}


void initWH(real **W, real **Htras, int N, int M, int K)
{
	int i,j;
	int ii, jj;
	
	int seedi;
	
	FILE *fd;

	/* Generated random values between 0.00 - 1.00 */	
	//fd = fopen("/dev/urandom", "r");
	//fread( &seedi, sizeof(int), 1, fd); 
	//fclose (fd);
	//srand( seedi ); 
	
	for (i=0; i<N; i++)
		for (j=0; j<K; j++)
			W[i][j] = ((real)(rand()))/RAND_MAX;
			//W[i][j] = (real)(i);

	for (i=0; i<M; i++)
                for (j=0; j<K; j++)
			Htras[i][j] = ((real)(rand()))/RAND_MAX;
			//Htras[i][j] = (real)(i);
#ifdef DEBUG
	/* Added to debug */
	FILE *fIn;
	int size_W = N*K;
	fIn = fopen("w_bin.bin", "r");
	fread(&W[0][0], sizeof(real), size_W, fIn);
	fclose(fIn);

	int size_H = M*K;
	fIn = fopen("h_bin.bin", "r");
	fread(&Htras[0][0], sizeof(real), size_H, fIn);
	fclose(fIn);
#endif
}

void printMATRIX(real **m, int I, int J)
{
	int i, j;
	
	printf("--------------------- matrix --------------------\n");
	printf("             ");
	for (j=0; j<J; j++){
		if (j<10)
			printf("%i      ", j);
		else if (j<100)
			printf("%i     ", j);
		else 
			printf("%i    ", j);
	}
	printf("\n");

	for (i=0; i<I; i++){
		if (i<10)
			printf("Line   %i: ", i);
		else if (i<100)
			printf("Line  %i: ", i);
		else
			printf("Line %i: ", i);
		for (j=0; j<J; j++)
			printf("%5.4f ", m[i][j]);
		printf("\n");
	}
}

void print_WH(real **W, real **Htras, int N, int M, int K)
{
	int i,j;	
	
	for (i=0; i<N; i++){
		printf("W[%i]: ", i);
		for (j=0; j<K; j++)
			printf("%f ",W[i][j]);
		printf("\n");
	}

	for (i=0; i<K; i++){
		printf("H[%i]: ", i);
        	for (j=0; j<M; j++)
			printf("%f ",Htras[j][i]);
		printf("\n");
	}
}

real **getV(char* file_name, int N, int M)
{
	char *data;
	FILE *fIn;
	
	/* Local variables */
	int i, j;
	int size_V;
	real **V;

	fIn = fopen(file_name, "r");

	V = get_memory2D(N, M);
	size_V = N*M;

#ifndef RANDOM 
	if (sizeof(real)==sizeof(float))
	{
		fread(&V[0][0], sizeof(float), size_V, fIn);
		fclose(fIn);
	} else {
		int i, j;
		float *Vaux = (float*)malloc(size_V*sizeof(float));
		fread(&Vaux[0], sizeof(float), size_V, fIn);
		fclose(fIn);
		for (i=0; i<N; i++)
			for (j=0; j<M; j++)
				V[i][j] = Vaux[i*M+j];
	}
#else
	/* Generated random values between 0.00 - 1.00 */
	FILE *fd;
	int seedi;
        //fd = fopen("/dev/urandom", "r");
        //fread( &seedi, sizeof(int), 1, fd);
        //fclose (fd);
        //srand( seedi );

        for (i=0; i<N; i++)
                for (j=0; j<M; j++)
                        V[i][j] = ((real)(rand()))/RAND_MAX;

#endif	
	return(V);
}


/* Gets the difference between matrix_max_index_h and conn_last matrices. */
int get_difference( unsigned char *classification, unsigned char *last_classification, int nx)
{
	int diff, difference;
	int conn, conn_last;
	int i, j;
	
	diff = 0;
	for(i=0; i<nx; i++)
		for(j=i+1; j<nx; j++) {
			conn = (int)( classification[j] == classification[i] );
			conn_last = (int) ( last_classification[j] == last_classification[i] );
			diff += ( conn != conn_last );
		}

	return (diff);
}

/* Get consensus from the classificacion vector */
void get_consensus( unsigned char *classification, unsigned char *consensus, int nx )
{
	unsigned char conn;
	int i, j,ii;
	
	ii = 0;
	for(i=0; i<nx; i++)
		for(j=i+1; j<nx; j++) {
			conn = ( classification[j] == classification[i] );
			consensus[ii] += conn;
			ii++;
		}
}

/* Obtain the classification vector from the Ht matrix */
void get_classification(real **Htras, unsigned char *classification, int M, int K)
{
	int i, j;
	
	real max;
	
	for (i=0; i<M; i++){
		max = 0.0;
		for (j=0; j<K; j++)
			if (Htras[i][j] > max)
			{
				classification[i] = (unsigned char)(j);
				max = Htras[i][j];
			}
	}
}


void adjust_WH(real **W, real **Ht, int N, int M, int K)
{
	int i, j;
	
	for (i=0; i<N; i++)
		for (j=0; j<K; j++)
			if (W[i][j]<eps)
				W[i][j]=eps;
				
	for (i=0; i<M; i++)
		for (j=0; j<K; j++)
			if (Ht[i][j]<eps)
				Ht[i][j]=eps;				 
}


real get_Error(real **V, real **W, real **Htras, int N, int M, int K)
{
	/*
	* norm( V-WH, 'Frobenius' ) == sqrt( sum( diag( (V-WH)'* (V-WH) ) )
	* norm( V-WH, 'Frobenius' )**2 == sum( diag( (V-WH)'* (V-WH) ) )
	*/

	/*
	* d[1..m] = diag( (V-WH)t * (V-WH) )
	*
	* is equivalent to:
	*     for i=1..m
	*         d[i] = sum( V-WH[:,i] .* V-WH[:,i] )
	*
	*
	* is equivalent to; error = sum( ( V-Vnew[:,i] .* V-Vnew[:,i] )
	*
	*/


	real error, tot_error;
	int i, j, k;
	real Vnew;
	
	error=0.0;
	for(i=0; i<N; i++)
	{
		for(j=0; j<M; j++){
			Vnew = 0.0;
			for(k=0; k<K; k++)
				Vnew+=W[i][k]*Htras[j][k];
			error+=(V[i][j]-Vnew)*(V[i][j]-Vnew);
		}
	}
	
	return(error);


}


void writeSolution(real **W, real **Ht, unsigned char *consensus, int N, int M, int K, int nTests)
{
	int i, j;
	FILE *fOut;
	char file[100];
	real **H;
	
	H = get_memory2D(K, M);
	for (i=0; i<K; i++)
		for (j=0; j<M; j++)
			H[i][j] = Ht[j][i];
	
	sprintf(file,"solution-NMFLeeSeung_%i", K);
	fOut = fopen(file, "w");
	fwrite( &N, sizeof(int), 1, fOut);
	fwrite( &M, sizeof(int), 1, fOut);
	fwrite( &K, sizeof(int), 1, fOut);
	fwrite( W[0], sizeof(real), N*K, fOut);
	fwrite( H[0], sizeof(real), K*M, fOut);
	fwrite( &nTests, sizeof(int), 1, fOut);
	fwrite( consensus, sizeof(unsigned char), (M*(M-1))/2, fOut);
	fclose( fOut );
	
	delete_memory2D(H);
}

double tW0=0.0, tW1=0.0, tW2=0.0, tW3=0.0, tW4=0.0;
double tH0=0.0, tH1=0.0, tH2=0.0, tH3=0.0, tH4=0.0;

void nmf_GPU(int niter, real *d_V, real *d_WH, real *d_W, real *d_Htras, real *d_Waux, real *d_Haux,
	real *d_accW, real *d_accH,
	int N, int M, int K)
{
#ifdef GPU
	int iter;
	
	int i, j, k;

	real diff, tot_diff;
	real Vn;

	double t0, t1;

	/*************************************/
	/*                                   */
	/*      Main Iterative Process       */
	/*                                   */
	/*************************************/
	for (iter=0; iter<niter; iter++)
	{

		/*******************************************/
		/*** H = H .* (W'*(V./(W*H))) ./ accum_W ***/
		/*******************************************/
t0 = gettime();
		W_mult_H(d_WH, d_W, d_Htras, N, M, K);		/* WH = W*H */
t1 = gettime(); tH0+=t1-t0; t0 = gettime();
		V_div_WH(d_V, d_WH, N, M );			/* WH = (V./(W*H) */
t1 = gettime(); tH1+=t1-t0; t0 = gettime();
		accum(d_accW, d_W, N, K); 			/* Reducir a una columna */
t1 = gettime(); tH2+=t1-t0; t0 = gettime();
		Wt_mult_WH(d_Haux, d_W, d_WH, N, M, K);		/* Haux = (W'* {V./(WH)} */
t1 = gettime(); tH3+=t1-t0; t0 = gettime();
		mult_M_div_vect(d_Htras, d_Haux, d_accW, M, K);	/* H = H .* (Haux) ./ accum_W */
t1 = gettime(); tH4+=t1-t0;

		/*******************************************/
		/*** W = W .* ((V./(W*H))*H') ./ accum_H ***/
		/*******************************************/
t0 = gettime();
		W_mult_H(d_WH, d_W, d_Htras, N, M, K);		/* WH = W*H */
t1 = gettime(); tW0+=t1-t0; t0 = gettime();
		V_div_WH(d_V, d_WH, N, M );			/* WH = (V./(W*H) */
t1 = gettime(); tW1+=t1-t0; t0 = gettime();
		WH_mult_Ht(d_Waux, d_WH, d_Htras, N, M, K);	/* Waux =  {V./(W*H)} *H' */
t1 = gettime(); tW2+=t1-t0; t0 = gettime();
		accum(d_accH, d_Htras, M, K); 			/* Reducir a una columna */
t1 = gettime(); tW3+=t1-t0; t0 = gettime();
		mult_M_div_vect(d_W, d_Waux, d_accH, N, K);	/* W = W .* Waux ./ accum_H */
t1 = gettime(); tW4+=t1-t0;

	}
#endif
}


real nmf(int niter, real **V, real **WH, real** W, real** Htras, real **Waux, real **Haux, 
	real *acumm_W, real *acumm_H,
	int N, int M, int K)
{
	int iter;
	
	int i, j, k;

	real diff, tot_diff;
	real Vn;

	real ONE  = 1.0;
	real ZERO = 0.0;

	/*************************************/
	/*                                   */
	/*      Main Iterative Process       */
	/*                                   */
	/*************************************/
	
	for (iter=0; iter<niter; iter++)
	{
	
		/*******************************************/
		/*** H = H .* (W'*(V./(W*H))) ./ accum_W ***/
		/*******************************************/
	
		/* WH = W*H */
#ifndef BLAS  
		for (i=0; i<N; i++)
			for (j=0; j<M; j++)
			{
				WH[i][j] = 0.0;
				for(k=0; k<K; k++)
				{
					WH[i][j] +=W[i][k]*Htras[j][k];
				}
				WH[i][j] = V[i][j]/WH[i][j]; /* V./(W*H) */
			}
#else

#ifdef ATLAS
		cblas_rgemm( CblasRowMajor, CblasNoTrans, CblasTrans, 
			N,				/* [m] */ 
			M,				/* [n] */
			K,				/* [k] */
			1, 				/* alfa */ 
			&W[0][0], K, 			/* A[m][k], num columnas (lda) */
			&Htras[0][0], K,		/* B[k][n], num columnas (ldb) */
			0,				/* beta */ 
			&WH[0][0], M			/* C[m][n], num columnas (ldc) */
		);
#endif /* ATLAS */

#ifdef FATLAS
		rgemm( "T", "N", 
			&M,				/* [n] */
			&N,				/* [m] */ 
			&K,				/* [k] */
			&ONE, 				/* alfa */ 
			&Htras[0][0], &K,		/* B[k][n], num columnas (ldb) */
			&W[0][0], &K, 			/* A[m][k], num columnas (lda) */
			&ZERO,				/* beta */ 
			&WH[0][0], &M			/* C[m][n], num columnas (ldc) */
		);

#endif /* FATLAS */
		#pragma omp parallel for private(i, j)
		for (i=0; i<N; i++)
			for (j=0; j<M; j++)
			{
				WH[i][j] = V[i][j]/WH[i][j]; /* V./(W*H) */
			}		


#endif // BLAS

		/* Reducir a una columna */
		for (j=0; j<K; j++)
			acumm_W[j]  = W[0][j];

		for (i=1;i<N; i++)
			for (j=0; j<K; j++)
				acumm_W[j] += W[i][j];



#ifndef BLAS
		/* Haux = (W'* {V./(WH)} */
		/* H = H .* (Haux) ./ accum_W */
		for (j=0; j<M; j++)
			for (i=0; i<K; i++)
				Haux[j][i] = 0.0;

		for (k=0; k<N; k++)
			for (i=0; i<K; i++)
				for (j=0; j<M; j++)
					Haux[j][i] += W[k][i]*WH[k][j];

		for (j=0; j<M; j++)
			for (i=0; i<K; i++)
				Htras[j][i] = Htras[j][i]*Haux[j][i]/acumm_W[i]; /* H = H .* (Haux) ./ accum_W */

#else 

#ifdef ATLAS
		cblas_rgemm( CblasColMajor, CblasNoTrans, CblasTrans,
			K,				/* [m] */
			M,				/* [n] */
			N,				/* [k] */
			1,				/* alfa */
			&W[0][0], K,			/* A[m][k], num columnas (lda) */
			&WH[0][0], M,			/* B[k][n], num columnas (ldb) */
			0,                      	/* beta */
			&Haux[0][0], K			/* C[m][n], num columnas (ldc) */
		);
#endif /* ATLAS */

#ifdef FATLAS
		rgemm( "N", "T",
			&K,				/* [m] */
			&M,				/* [n] */
			&N,				/* [k] */
			&ONE,				/* alfa */
			&W[0][0], &K,			/* A[m][k], num columnas (lda) */
			&WH[0][0], &M,			/* B[k][n], num columnas (ldb) */
			&ZERO,                      	/* beta */
			&Haux[0][0], &K			/* C[m][n], num columnas (ldc) */
		);
#endif /* FATLAS */

		#pragma omp parallel for private(i, j)
		for (j=0; j<M; j++){
			for (i=0; i<K; i++)
				Htras[j][i] = Htras[j][i]*Haux[j][i]/acumm_W[i]; /* H = H .* (Haux) ./ accum_W */
		}


#endif // BLAS

		
		/* Reducir a una columna */
		for (j=0; j<K; j++)
			acumm_H[j]  = Htras[0][j];

		for (i=1;i<M; i++)
			for (j=0; j<K; j++)
			acumm_H[j] += Htras[i][j];

		/*******************************************/
		/*** W = W .* ((V./(W*H))*H') ./ accum_H ***/
		/*******************************************/


		/* WH = W*H */
		/* V./(W*H) */
#ifndef BLAS
		for (i=0; i<N; i++)
		{
			for (j=0; j<M; j++)
			{
				WH[i][j] = 0.0;
				for(k=0; k<K; k++)
				{
					WH[i][j] +=W[i][k]*Htras[j][k];
				}
				WH[i][j] = V[i][j]/WH[i][j]; /* V./(W*H) */
			}
		}
#else

#ifdef ATLAS
		cblas_rgemm( CblasRowMajor, CblasNoTrans, CblasTrans, 
			N,				/* [m] */ 
			M, 				/* [n] */
			K,				/* [k] */
			1, 				/* alfa */ 
			&W[0][0], K,		 	/* A[m][k], num columnas (lda) */
			&Htras[0][0], K,		/* B[k][n], num columnas (ldb) */
			0,				/* beta */ 
			&WH[0][0], M			/* C[m][n], num columnas (ldc) */
		);
#endif /* ATLAS */

#ifdef FATLAS
		rgemm( "T", "N", 
			&M, 				/* [n] */
			&N,				/* [m] */ 
			&K,				/* [k] */
			&ONE, 				/* alfa */ 
			&Htras[0][0], &K,		/* B[k][n], num columnas (ldb) */
			&W[0][0], &K,		 	/* A[m][k], num columnas (lda) */
			&ZERO,				/* beta */ 
			&WH[0][0], &M			/* C[m][n], num columnas (ldc) */
		);

#endif /* FATLAS */

		#pragma omp parallel for private(i, j)
                for (i=0; i<N; i++)
                        for (j=0; j<M; j++)
                        {
                                WH[i][j] = V[i][j]/WH[i][j]; /* V./(W*H) */
                        }


#endif // BLAS


		/* Waux =  {V./(W*H)} *H' */
		/* W = W .* Waux ./ accum_H */
#ifndef BLAS
		for (i=0; i<N; i++)
		{
			for (j=0; j<K; j++)
			{
				Waux[i][j] = 0.0;
				for(k=0; k<M; k++)
				{
					Waux[i][j] += WH[i][k]*Htras[k][j];
				}
				W[i][j] = W[i][j]*Waux[i][j]/acumm_H[j]; /* W = W .* Waux ./ accum_H */
			}
		}
#else

#ifdef ATLAS
		cblas_rgemm( CblasRowMajor, CblasNoTrans, CblasNoTrans, 
			N,				/* [m] */ 
			K, 				/* [n] */
			M,				/* [k] */
			1, 				/* alfa */ 
			&WH[0][0], M,		 	/* A[m][k], num columnas (lda) */
			&Htras[0][0], K,		/* B[k][n], num columnas (ldb) */
			0,				/* beta */ 
			&Waux[0][0], K			/* C[m][n], num columnas (ldc) */
		);

#endif /* ATLAS */

#ifdef FATLAS
		rgemm( "N", "N", 
			&K, 				/* [n] */
			&N,				/* [m] */ 
			&M,				/* [k] */
			&ONE, 				/* alfa */ 
			&Htras[0][0], &K,		/* B[k][n], num columnas (ldb) */
			&WH[0][0], &M,		 	/* A[m][k], num columnas (lda) */
			&ZERO,				/* beta */ 
			&Waux[0][0], &K			/* C[m][n], num columnas (ldc) */
		);

#endif /* FATLAS */

		#pragma omp parallel for private(i, j)
		for (i=0; i<N; i++)
		{
			for (j=0; j<K; j++)
			{
				W[i][j] = W[i][j]*Waux[i][j]/acumm_H[j]; /* W = W .* Waux ./ accum_H */
			}
		}


#endif // BLAS
	}

}

int main(int argc, char *argv[])
{
	int nTests, niters;

	int i,j;
	real obj;

	real **V;
	real **W, **W_best;
	real **Htras, **Htras_best;
	unsigned char *classification, *last_classification;
	unsigned char *consensus; /* upper half-matrix size M*(M-1)/2 */

	/* Auxiliares para el computo */
	real **WH;
	real **Haux,  **Waux;
	real *acumm_W;
	real *acumm_H;
	
	//For GPU
	real *d_V;
	real *d_W;
	real *d_Htras;
	real *d_WH;
	real *d_Haux;
	real *d_Waux;
	real *d_acumm_W;
	real *d_acumm_H;

	int N;
	int M;
	int K;
	int stop_threshold, stop;
	char file_name[255];
	int test, iter;
	int diff, inc;
	
	double time0, time1;
	double timeGPU2CPU, timeGPU1, timeGPU0;
	
	real error;
	real error_old=9.99e+50;

        setbuf( stdout, NULL );
	
	if (argc==7){
		strcpy(file_name, argv[1]);
		N              = atoi(argv[2]);
		M              = atoi(argv[3]);
		K              = atoi(argv[4]);
		nTests         = atoi(argv[5]);
		stop_threshold = atoi(argv[6]);
	} else {
	 	printf("./exec dataInput.bin N M K nTests stop_threshold (argc=%i %i)\n", argc, atoi(argv[2]));

		return(0);
	}

	V = getV(file_name, N, M);
	printf("file=%s\nN=%i M=%i K=%i nTests=%i stop_threshold=%i\n", file_name, N, M, K, nTests, stop_threshold);	

	W   = get_memory2D(N, K);
	Htras  = get_memory2D(M, K);
	W_best   = get_memory2D(N, K);
	Htras_best  = get_memory2D(M, K);
	
	WH = get_memory2D(N, M);
	
	Haux  = get_memory2D(M, K);
	Waux  = get_memory2D(N, K);
	
	acumm_W  = get_memory1D(K);
	acumm_H  = get_memory1D(K);
	
	classification      = get_memory1D_uchar(M);
	last_classification = get_memory1D_uchar(M);
	consensus           = get_memory1D_uchar(M*(M-1)/2);

#ifdef GPU
	cublasInit();
	cudaMalloc((void **)&d_V,      N*M*sizeof(real));
	cudaMemcpy(d_V, V[0], N*M*sizeof(real), cudaMemcpyHostToDevice);
	cudaMalloc((void **)&d_W,      N*K*sizeof(real));
	cudaMalloc((void **)&d_Htras,  M*K*sizeof(real));
	cudaMalloc((void **)&d_WH,     N*M*sizeof(real));

	cudaMalloc((void **)&d_Waux,   N*K*sizeof(real));
	cudaMalloc((void **)&d_Haux,   M*K*sizeof(real));

	cudaMalloc((void **)&d_acumm_W,K*sizeof(real));
	cudaMalloc((void **)&d_acumm_H,K*sizeof(real));
#endif

	/**********************************/
	/******     MAIN PROGRAM     ******/
	/**********************************/
	time0 = gettime();

	for (test=0; test<nTests; test++)
	{
		/* Init W and H */
		initWH(W, Htras, N, M, K);
#ifdef GPU
		timeGPU0 = gettime();
		cudaMemcpy(d_W,     W[0],     N*K*sizeof(real), cudaMemcpyHostToDevice);
		cudaMemcpy(d_Htras, Htras[0], K*M*sizeof(real), cudaMemcpyHostToDevice);
		timeGPU1 = gettime();
		timeGPU2CPU += timeGPU1-timeGPU0;
#endif	

		niters = 2000/NITER_TEST_CONV;

		stop   = 0;
		iter   = 0;
		inc    = 0;
		while (iter<niters && !stop)
		{
			iter++;
#ifndef GPU
			/* Main Proccess of NMF Brunet */
			nmf(NITER_TEST_CONV, 
				V, WH, W, Htras, Waux, Haux, acumm_W, acumm_H,
				N, M, K);

			/* Adjust small values to avoid undeflow: h=max(h,eps);w=max(w,eps); */
			adjust_WH(W, Htras, N, M, K);
#else
			/* Main Proccess of NMF Brunet */
			nmf_GPU(NITER_TEST_CONV, 
				d_V, d_WH, d_W, d_Htras, d_Waux, d_Haux, d_acumm_W, d_acumm_H,
				N, M, K);

			/* Adjust small values to avoid undeflow: h=max(h,eps);w=max(w,eps); */
			adjust_WH_GPU(d_W, d_Htras, N, M, K);

			timeGPU0 = gettime();
			cudaMemcpy(W[0],     d_W,     N*K*sizeof(real), cudaMemcpyDeviceToHost);
			cudaMemcpy(Htras[0], d_Htras, K*M*sizeof(real), cudaMemcpyDeviceToHost);
			timeGPU1 = gettime();
			timeGPU2CPU += timeGPU1-timeGPU0;
#endif
			/* Test of convergence: construct connectivity matrix */
			get_classification( Htras, classification, M, K);

			diff = get_difference( classification, last_classification, M);
			matrix_copy1D_uchar( classification, last_classification, M );

			if( diff > 0 ) 	/* If connectivity matrix has changed, then: */
				inc=0;  /* restarts count */
			else		/* else, accumulates count */
				inc++;

			if (verbose)
				printf("iter=%i inc=%i number_changes=%i\n", iter*NITER_TEST_CONV, inc, 2*diff);

			/* Breaks if connectivity stops changing: NMF converged */
			if ( inc > stop_threshold ) {
				stop = 1;
			}	
		}

		/* Get Matrix consensus */
		get_consensus( classification, consensus, M );

		/* Get variance of the method error = |V-W*H| */
		error = get_Error(V, W, Htras, N, M, K);
		if (error<error_old){
			printf("Better W and H, Error %e Test=%i, Iter=%i\n", error, test, iter*NITER_TEST_CONV);
			matrix_copy2D(W, W_best, N, K);
			matrix_copy2D(Htras, Htras_best, M, K);
			error_old = error;
		}		
	}
	time1 = gettime();
	/**********************************/
	/**********************************/

	
#ifdef GPU 
	printf("\n\n\n EXEC TIME %f (s). (CPU2GPU %f s)      N=%i M=%i K=%i Tests=%i (%i)\n", (time1-time0)/1000000, timeGPU2CPU/1000000, N, M, K, nTests, sizeof(real));
#else
	printf("\n\n\n EXEC TIME-ATLAS %f (s).                N=%d M=%d K=%i Tests=%d (%d)\n", (time1-time0)/1000000, N, M, K, nTests, (int)sizeof(real));
#endif

	printf("tH0=%f tH1=%f tH2=%f tH3=%f\n", tH0/1000000, tH1/1000000, tH2/1000000, tH3/1000000);
	printf("tW0=%f tW1=%f tW2=%f tW3=%f\n", tW0/1000000, tW1/1000000, tW2/1000000, tW3/1000000);

	/* Write the solution of the problem */
	writeSolution(W_best, Htras_best, consensus, N, M, K, nTests);

	/* Free memory used */
	delete_memory2D(W);
	delete_memory2D(Htras);
	delete_memory2D(W_best);
	delete_memory2D(Htras_best);	
	delete_memory2D(WH);
	delete_memory2D(Waux);
	delete_memory2D(Haux);
	delete_memory1D(acumm_W);
	delete_memory1D(acumm_H);
	delete_memory1D_uchar(classification);
	delete_memory1D_uchar(last_classification);
	delete_memory1D_uchar(consensus);

}
