int a,b;

int* f() {
    if(a > 10) return &a;
    else return &b;
}

int main() {
    int *p;
    p = f();

    return 0;
}