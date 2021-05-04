#include <stdio.h>

unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int count() {

	int z = 0;

	do{
		for(int i = 0; i < 100; i++){
			for(int j = 0; j < 100; j++) {
				
				int a = 0;
				while(a < 100) {
				a++;
				}
			}
z++;
		}
	} while(z < 1000);

return z;

}

int main() {

	int z = count();
	
	printf("Z: %d\n", z);
	printf("CNT: %llu\n", cnt);
}
