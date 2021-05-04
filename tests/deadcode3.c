#include <stdio.h>

unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int count() {
	
	int z = 0;
	int a = 1;
	int q = -1;
	int t = 0;
	int b = 0;
	int c = 0;
	int d = 0;
	int e = 0;
	int f = 0;
	int g = 0;
	int h = 0;

	if(a < 10)
	q += a + z;
	if(a < 10)
	t += a + q;
	if(a < 10)
	b += t + q;
	if(a < 10)
	c += b + t;
	if(a < 10)
	d += c + b;
	if(a < 10)
	e += d + c;
	if(a < 10)
	f += e + d;
	if(a < 10)
	g += f + e;

return g;

}

int main() {

	int z = count();
	
	printf("Z: %d\n", z);
	printf("CNT: %llu\n", cnt);
}
