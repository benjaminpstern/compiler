int f(int index) {
    return 2 * index + 1;
}
void main() {
    int A[10];
    int i;
    i = 0;
    while (i < 10) {
        A[i] = f(i);
        i = i + 1;
    }
    i = 0;
    while (i < 10) {
        write(A[i]);
        i = i + 1;
    }
}
