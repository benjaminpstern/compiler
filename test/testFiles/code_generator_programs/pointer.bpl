int a;
void f(int x) {
    int* xptr;
    xptr = &x;
    *xptr = *xptr + 1;
    write(x);
}
void main() {
    int x;
    int* xptr;
    int* aptr;
    x = 5;
    xptr = &x;
    aptr = &a;
    write(x);
    *xptr = 6;
    write(x);
    f(8);
    *aptr = 10;
    write(a);
    *aptr = *aptr + 1;
    write(a);
    writeln();
}
