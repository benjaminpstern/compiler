void main() {
    int A[5];
    int* p;
    p = &(A[3]);
    *p = 5;
    write(A[3]);
}
