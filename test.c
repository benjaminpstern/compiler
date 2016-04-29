#include <stdio.h>
int main() {
    int x = 0;
    int* y;
    y = &x;
    *y = 5;
    printf("%d\n", *y);
}
