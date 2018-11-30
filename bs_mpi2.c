#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "generatelist.h"

#define A 65536 // 2^16
#define N 100000
#define BUCKET_NUM 4 // 2^2
#define MASTER 0

void quickSort(int num[], int low, int high);

// Quick sort
void quickSort(int num[], int low, int high)
{
	int temp;
	int i = low, j = high;
	
	if (low < high){
		temp = num[low];
	
		while (i < j){
			while (j > i && num[j] >= temp) j--;
		
			if (i < j){
				num[i] = num[j];
				++i;
			}
		
			while (i < j && num[i] < temp) i++;

			if (i < j){
				num[j] = num[i];
				--j;
			}
		}
		num[i] = temp;
		quickSort(num, low, i-1);
		quickSort(num, i+1, high);
	}
}	

int main(int argc, char * argv[])
{
	int arr[N];
	int thread;
	int size=0;
	double start, end;

	start = clock();
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &thread);

	// Generate a random array
	if(thread == MASTER)
		generatelist(arr, N, BUCKET_NUM, A);
	
	// sort the array use bucket sort function

	int s_bucket[BUCKET_NUM][N/BUCKET_NUM]; // small buckets

	int bucket[BUCKET_NUM][N/BUCKET_NUM]; // big buckets
	// record the number of elements in each bucket
	int num_in[BUCKET_NUM];
	int bt[N/BUCKET_NUM]; // receive the result from small bucket
	// initialize
	for (int m = 0; m < BUCKET_NUM; m++){
		num_in[m] = 0;
		for (int n = 0; n < N/BUCKET_NUM; n++)
			bucket[m][n] = 0;
	}
	// insert into bucket matrix
	MPI_Bcast(&arr, N, MPI_INT, 0, MPI_COMM_WORLD);
	int index = 0;
	int j = 0;
	
	for (j = (thread*N/BUCKET_NUM); j < ((thread+1)*N/BUCKET_NUM); j++){
		int temp = arr[j];
		index = arr[j] >> 14;
		if (thread == index){
			s_bucket[index][num_in[index]] = temp;
			num_in[index]++;
		}
	}
	// this function make all small buckets are emptied into N/BUCKET_NUM big buckets
	MPI_Alltoall(&s_bucket, N/BUCKET_NUM, MPI_INT, &bt, N/BUCKET_NUM, MPI_INT, MPI_COMM_WORLD);

	// sorting numbers in each bucket

	quickSort(bt, 0, N/BUCKET_NUM-1);
	MPI_Gather(&bt, N/BUCKET_NUM, MPI_INT, &bucket, N/BUCKET_NUM, MPI_INT, 0, MPI_COMM_WORLD);
	end = clock();

	MPI_Finalize();

	/*if (thread == MASTER){
		for (int i = 0; i < BUCKET_NUM; i++)
			for (int j = 0; j < N/BUCKET_NUM; j++)
				printf("%d ", bucket[i][j]);
	}*/
	
	if (thread == MASTER)
		printf("\nThe total time of sorting is %f.\n", end-start);
	return 0;
}
