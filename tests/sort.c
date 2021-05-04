#define len 32

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


int main() {
	
	int arr[len];
	int dest[len];
	int count = 0;
	int idx = -1;



	//srand(time(NULL));

	for(int i = 0; i < len; i++) arr[i] = 1;//rand() % 1024;

	for(int i = 0; i < len; i++){
		int max = -1;
		for(int j = 0; j < len; j++){
			if(arr[j] > max){
				max = arr[j];
				idx = j;
			}
		}

		dest[count] = max;
		arr[idx] = -1;
		count++;
	}

	for(int i = 0; i < len; i++) printf("%d ", dest[i]);
	printf("\n");	
}
