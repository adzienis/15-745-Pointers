#include <stdio.h>

unsigned long long int volatile cnt = 0; // INCLUDE THIS IN YOUR MICROBENCHMARK

int main(){

  int res = 0;
  int same = 5;

  int x = 0;
  int y = 0;
  for (int a = 0; a < same; a++) {
    int a1 = 0;
    int a2 = 0;
    for (int b = 0; b < same; b++) {
      int b1 = 0;
      int b2 = 0;
      for (int c = 0; c < same * same; c++) {
        int c1 = 0;
        int c2 = 0;
        for (int d = 0; d < same + 2; d++) {
          int d1 = 0;
          int d2 = 0;
          for (int e = 0; e < same + same; e++) {
            int e1 = 0;
            int e2 = 0;
            for (int f = 0; f < 100; f++) {
              int f1 = 0;
              int f2 = 0;
              for (int g = 0; g < 10; g++) {
                int g1 = 0;
                int g2 = 0;
                for (int h = 0; h < 5 * same; h++) {
                  g2 += h;
                  g1 = 5;
                }
                f2 += g2 + d2 + c2;
                res += f2;
              }
              f1 = 10;
              f2 += 1;
              e2 = f2 + a2 + b2;
            }
            d1 = e1;
            d2 += 1 * a2;
          }
stupid:
          c1 = same;
          c2 += same + b + b2;
        }
        b1 = x + same;
        b2 = a2 * 5;
      }
stupid2:
      a1 = same * 2;
      a2 += 3;
    }
    y += 1;
  }

  printf("%d %d %d\n", res, x, y);

	printf("CNT: %llu\n", cnt);

  return res;
}
