#include <stdio.h>

int foo(int a, int b, int c) {
    int e, g, m;
    int i = 0;
    int d = a+b;
    int f = 5;
    if(a > 5) {
        g = d + c;
        m = g + e;
    } else {
        while(b < 5) {
            b = b + 1;
            f = b + c;
        }
        g = a + 3;
    }

    e = b + c;
    int h = g + e;
    int k = h + f;
    int n = k + m;
    return n;
}