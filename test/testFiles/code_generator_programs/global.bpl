int x;
void set_x_to_n(int n) {
    x = n;
}
void set_x_to_5() {
    x = 5;
}
void main() {
    set_x_to_5();
    write(x);
    x = 6;
    write(x);
    set_x_to_n(12);
    write(x);
}
