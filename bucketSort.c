#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generatelist.h"

#define A 1024 // 2^10
#define N 1000000
#define BUCKET_NUM 16 // 2^4

void bucketSort(int arr[]);
void quickSort(int num[], int low, int high);

void bucketSort(int arr[])
{
	// Make unsorted numbers into buckets
	// (Let bucket capable for x numbers -> N/16)
	
	// bucket matrix
	int bucket[16][N/16];
	// record the number of elements in each bucket
	int num_in[16];
	// initialize
	for (int m = 0; m < 16; m++){
		num_in[m] = 0;
		for (int n = 0; n < N/16; n++)
			bucket[m][n] = 0;
	}
	// insert into bucket matrix
	for (int j = 0; j < N; j++){
		int temp = arr[j];
		int index = arr[j] >> 6;
		bucket[index][num_in[index]] = temp;
		num_in[index]++;
	}

	// sorting numbers in each bucket
	for (int i = 0; i < BUCKET_NUM; ++i)
		quickSort(bucket[i], 0, N/16-1);
	
	// print the sorted number every 10 in one line
	int a = 0;
	for (int i = 0; i < 16; i++){
		for (int j = 0; j < N/16; j++){
			if (a != 10){
				printf("%d ", bucket[i][j]);
				a++;
			}else {
				printf("\n");
				a = 0;
			}
		}
	} 	

}

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
	time_t start, end;
	// Generate a random array
	start = time(NULL);
	generatelist(arr, N, BUCKET_NUM, A);
	// sort the array use bucket sort function
	bucketSort(arr);
	end = time(NULL);
	printf("\nThe total time of sequential sort is %f.\n", difftime(end, start));
}
