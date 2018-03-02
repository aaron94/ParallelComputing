// Aaron O'Connor | 13371201
// C program 
//05 Jan 2017

// compile with: gcc -o ass4 ass4.c -fopenmp



#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

main(){

	int N = 500;
	int N_Threads = 25;	//# threads should be divisible by N AND <= N		
	int i, j, k, threads, cols, first, last;
	double norm, norm_col, sum_col, sum, elapsedTime;
	struct timeval t1, t2;

	void initializeMatrix(int N, double **matrix);
	void initializeMatrixZero(int N, double **matrix);
	void printMatrix(int N, double **matrix);

	omp_set_num_threads(N_Threads);
	cols = N / N_Threads; //partition

	//Allocate Memory
	double **A = malloc(N * sizeof(double *));
	double **B = malloc(N * sizeof(double *));
	double **C = malloc(N * sizeof(double *));

	for(i=0; i<N; i++){
		A[i] = malloc(N * sizeof(double *));
		B[i] = malloc(N * sizeof(double *));
		C[i] = malloc(N * sizeof(double *));
	}

	initializeMatrix(N, A);
	initializeMatrix(N, B);
	initializeMatrixZero(N, C);

	printf("\n\nCompute the norm of the product of matrix A and B, with %d threads, when N = %d.", N_Threads, N);

	// printf("\n\nPrinting Matrix A:");
	// printMatrix(N, A);
	// printf("\n\nPrinting Matrix B:");
	// printMatrix(N, A);
	

	gettimeofday(&t1, NULL);

	#pragma omp parallel shared(A, B, C, norm_col) private(i, j, k, sum_col, norm, threads, sum, first, last)
	{

		threads = omp_get_thread_num();
		first = threads * cols;
		last = ((threads + 1) * cols) - 1;

		sum_col = 0.0;
		for(i = first; i <= last; i++){
			for(j = 0; j < N; j++){
				sum = 0.0;
				for(k=0; k < N; k++){
					sum += A[i][k] * B[k][j];
				}
				C[i][j] = sum;
				sum_col += sum;

				if(norm < sum_col){
					norm = sum_col;
				}

				#pragma omp critical

				if(norm_col < norm){
					norm_col = norm;
				}
			}
		}
	}

	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

	printf("\n\nNorm of matrix (AxB):			%.2f", norm_col);
	printf("\nTime taken:				%f miliseconds\n\n", elapsedTime);


	//**SERIAL COUNTERPART CALCULATIONS FOR COMPARISON**\\

	gettimeofday(&t1, NULL);

	for(i = 0; i < N; i ++){
		for(j = 0; j < N; j ++){
			sum = 0.0;
			for(k = 0; k < N; k ++){
				sum += A[i][k] * B[k][j];
			}
			C[i][j] = sum;
			sum_col += sum;
		}
	}

	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

    printf("Matrix multiplication on serial counterpart of program for time comparison.");

	printf("\n\nTime taken:				%f miliseconds\n\n\n", elapsedTime);
}


void initializeMatrix(int N, double **matrix){
	int i,j;
	for(i = 0; i < N; i++){
		for(j=0; j < N; j++){
			matrix[i][j] = 2;
		}
	}
}

void initializeMatrixZero(int N, double **matrix){
	int i,j;
	for(i = 0; i < N; i++){
		for(j=0; j < N; j++){
			matrix[i][j] = 0;
		}
	}
}

void printMatrix(int N, double **matrix){
	printf("\n\n");
	int i, j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			printf("   %.2f", matrix[i][j]);
		}
		printf("\n\n");
	}
}



