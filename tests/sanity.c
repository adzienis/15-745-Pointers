#include <stdio.h>


unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int main() {

	int x =0;
	int y = 10;
	x += y;
	y += x;
	int z = 0;
	int q = 0;

	for(int i = 0; i < 20000; i++){
	z = y + x; 
	q = y-x;
	}
	
	printf("CNT: %llu\n", cnt);
}
