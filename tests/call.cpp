#include <stdio.h>

int g;

void f(int *fp) { *fp = 10;}

int main() {
    int *p;
    p = &g;
    f(p);

    printf("asda");

    return 0;
}