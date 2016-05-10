int f(int i) {
    if (i < 100) {
        return 5 + 2 + i * 2;
    }
    else {
        return i + 3 + 4 + 54 + 283;
    }
}
void main() {
    int n;
    int i;
    int r;
    n = 500000000;
    i = 0;
    r = 0;
    while (i < n) {
        if (r > 1000) {
            r = r - f(r);
        }
        else {
            r = r + f(r);
        }
        i = i + 1;
    }
    write(r);
}
