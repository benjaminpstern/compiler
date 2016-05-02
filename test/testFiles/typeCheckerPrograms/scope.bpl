int z;
int func(int thing) {
    thing = thing + 1;
    return thing;
}
void main(int five) {
    int x;
    int y;
    {
        int z;
        int a;
    }
    {
        int b;
        int c;
        if (b) {
            int d;
            b = d;
        }
    }
    x = 1;
    y = 2;
    write(x+y);
}
