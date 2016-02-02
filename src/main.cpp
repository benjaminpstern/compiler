#include <stdio.h>
#include "sample.h"
int main() {
    sample s;
    s.variable = 5;
    printf("Hello world\n");
    printf("%f\n", s.func(4));
    return 0;
}
