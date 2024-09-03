#include <stdlib.h>
#include <stdio.h>
#include <time.h>
void multiply_block(double **C, double **A, double **B, int bi, int bj, int bk, int b) {
    // Perform block matrix multiplication
    for (int i = 0; i < b; i++) {
        for (int j = 0; j < b; j++) {
            for (int k = 0; k < b; k++) {
                C[bi + i][bj + j] += A[bi + i][bk + k] * B[bk + k][bj + j];
            }
        }
    }
}

int main(void){
		int i, j, k;
		struct timespec start, stop; 
		double time;
		int n = 4096; // matrix size is n*n
        int b = 16; // block size
        int m = n / b ; // # of blocks
		
		double **A = (double**) malloc (sizeof(double*)*n);
		double **B = (double**) malloc (sizeof(double*)*n);
		double **C = (double**) malloc (sizeof(double*)*n);
		for (i=0; i<n; i++) {
			A[i] = (double*) malloc(sizeof(double)*n);
			B[i] = (double*) malloc(sizeof(double)*n);
			C[i] = (double*) malloc(sizeof(double)*n);
		}
		
		for (i=0; i<n; i++){
			for(j=0; j< n; j++){
				A[i][j]=i;
				B[i][j]=i+j;
				C[i][j]=0;			
			}
		}
				
		if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
		
		// Your code goes here //
		// Matrix C = Matrix A * Matrix B //	
		//*******************************//
		for (i = 0; i < m; i++) {
            for (j = 0; j < m; j++) {
                for (k = 0; k < m; k++) {
                    multiply_block(C, A, B, i * b, j * b, k * b, b);
                }
            }
        }
		
		//*******************************//
		
		if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
		time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
		
        printf("block size = %d", b);
		printf("Number of FLOPs = %lu, Execution time = %f sec,\n%lf MFLOPs per sec\n", 2*n*n*n, time, 1/time/1e6*2*n*n*n);		
		printf("C[100][100]=%f\n", C[100][100]);
		
		// release memory
		for (i=0; i<n; i++) {
			free(A[i]);
			free(B[i]);
			free(C[i]);
		}
		free(A);
		free(B);
		free(C);
		return 0;
}
