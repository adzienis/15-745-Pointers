#include <stdio.h>

unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int count() {

	int a= 1230;
	int b= 0;
	int c = 0;
	int d = 0;
	int e = 0;
	int f = 10;
	int h= 30;
	int v = 40;
	int g = 0;

	for(int l = 0; l < 1000; l++){
		if(a > 0){
		if(f < 10) {
			if(g < 100) {
				b += a + c;
				c += b + c;
			} else {
			 d = e + f;
			 e = a + b;
			}
			for(int x = 0; x < 30; x++) {
				d++;
				e++;		
			}
		}
		} else {
		g += v + h;
			
		for(int i = 0; i < 30; i++){
			g -= i;
		}
		}

	}

	return g;

}

int main() {

	int z = count();
	
	printf("Z: %d\n", z);
	printf("CNT: %llu\n", cnt);
}
