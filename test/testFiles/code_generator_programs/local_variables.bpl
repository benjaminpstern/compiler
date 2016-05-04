int f(int x) {
    int a;
    int b;
    a = 5;
    b = 12;
    return x + a * b;
}
void main() {
    int x;
    int y;
    x = 5;
    y = 12;
    write(x);
    write(y);
    write(f(x));
}
