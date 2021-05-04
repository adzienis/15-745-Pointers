#include <stdio.h>

unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int compute (int a, int b)
{

	int x = 5;
	int y = 10;
	x += y + 1;
	int t = 100;

	int p = 10;
	int qw = 15;

	if(x == 5) {
	  t = p + 3;
	} else {
	  t= qw + 12312;
	}

	int q = t + 1;

	int z = 30;
	z += 123;
	int v = 10;
	z += v + 1;
	

	return x + q;
/*
  int result = (a/a);

  int q = 5;
  int p = 10;
  int t= 0;

  if(q == 10) t = q+p;
  else t = q+p;


  if(q == 10) {
  int z = 0;

  z += 10;
  }

  t += q;
  


  result *= (b/b);
  result += (b-b);
  result /= result;
  result -= result;
  return result;
  */
}

int main() {

	int res = compute(1 ,2);

	printf("res: %d\n", res);
	printf("CNT: %llu\n", cnt);
}
