int fact(int x) {
    if (x < 2) {
        return 1;
    }
    return x * fact(x - 1);
}
void main() {
    write(fact(5));
}
