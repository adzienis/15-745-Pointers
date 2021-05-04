#include <stdio.h>


unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int main() {
	/*
	int a = 30;
	int e= 0;
	a += e;
	e += a;
	int b = 1;
	b+=e;
	int c = 5;
	c+=e;
	for(int i =0; i < 10; i++){
	e = a;
	a = b+c;
	}
	*/
	/*
	int i = 0;
	int a = 9999999;
	a += i;
	int z = 0;
	do{
		printf("%d\n", i);
		if(i == 0) {
			i = 100;
			goto test;
		} else if(i == 5) {
			i = 69;
		goto test;
		}  else if(i == 7) {
			i = 1001;
		goto test;
		}
		i = a * z;
		i++;
	}
	while(i < 10);

test:
printf("%d\n", i);
	if(i) i= 2;
	*/
	
	int z = 0;
	int x= 0;
	int v = 0;
	int w[1024];
	int q = 69;
	int t = 999999;

	for(int i = 0; i < 10; i++) {
		x += 11;
		for(int j = 0; j < 100; j++) {
			q = 51;
			q = t+102102;
			if(x % 5) {
			continue;
			}
		 	z += 5;

			for(int k = 0; k < 100; k++) {
				v++;
			}

		}

	}

	printf("%d %d %d\n", z, x, v);
	
	
	printf("CNT: %llu\n", cnt);
}
