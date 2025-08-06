#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
int add(int a, int b);

int main() {
    printf("Hello, World!\n");
    int result = add(3, 4);
    printf("Result: %d\n", result);
    return 0;
}

int add(int a, int b) {
    return a + b;
}
