#include <stdio.h>

unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int bar (int n, int m) {
  int res = 0;
  int x = 0;
  int r = 100;
  int cran = 200;
  int bad = 0;
  for (int i = 0; i < n * m; i++) {
    x = 1300;
    int f = 30;
    for (int a = 0; a < n; a++) {
      int intermed = a;
      for (int b = 0; b < m; b++) {
        r = 1000;
        for (int c = 0; c < 151; c++) {
          f++;
          for (int d = 0; d < n; d++) {
            res++;
          }
        }
      }
      cran = r;
    }
    bad += f;
    int y = 1 + 1;
    res = 5 + y;
  }
  return res + x + cran + r + bad;
}

int foo (int n) {
  int res = 0;
  int y = n + 1;
  int z = 5;
  for (int i = 0; i < n; i++){
    z = n * n;
    int temp = y + z;
    int l = i + 1;
    res = z;
  }
  return res;
}


int main(){

  int x = 10;

  int res = 0;
  int q = 0;
  for (int i = 0; i < x; i++) {
    res = bar(x, x) + foo(x);
    for (int j = 0; j < x + 5; j++) {
      q = x;
    }
  }

  printf("%d %d\n", res, q);


	printf("CNT: %llu\n", cnt);

  return res;
}
