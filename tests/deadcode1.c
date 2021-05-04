#include <stdio.h>

unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int count() {

	int z = 0;
	int a = 1;
	int q = -1;
	int t = 0;
	for(int i = 0; i < 10000; i++){
		z +=a + 10;

			q += 10 + t;
			t += 10 + q;

		for(int j = 0; j < 100; j++) {
			z -= 1;
		if(z % 5) {
			q += 10  + t;
			t += 10  + q;
		}
			if(z  < -100) {
			goto end;
			}
		}
end:;
	}

return z;

}

int main() {

	int z = count();
	
	printf("Z: %d\n", z);
	printf("CNT: %llu\n", cnt);
}
