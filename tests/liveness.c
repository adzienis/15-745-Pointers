
int nirvana(int x, int y) {
    int xy = x * y;
    int temp = 150 * x;
    if (xy < x + y) {
        xy = nirvana(y, x);
    }
    else if (y == 0){
        xy = nirvana(temp, y);
    }
    else if (x == 0) {
        xy = nirvana(temp, x);
    }
    else {
        xy = nirvana(temp, temp);
    }
    return xy;
}

int nonsense(int m, int d){
    if (m < 0 || d < 0) {
        return 2;
    }
    int y = m * 2;
    if (3 * d == 2) {
        y = m + d;
    }
    else if (y < 3 * d) {
        y = nonsense(m - 2, d - 1);
    }
    else if (m - d == 0) {
        y = nirvana(y, d);
    }
    return y * 3;
}



int main(){
    int d = 2;
    int y = nonsense(d, 2 * d);
    return y * 2;
}
