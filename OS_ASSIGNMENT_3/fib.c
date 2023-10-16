#include <stdio.h>
#include <stdlib.h>

int fib(int val) {
    if (val<=2) {
        return 1;
    } else {
        return fib(val - 1) + fib(val - 2);
    }
}

int main() {
    int result = fib(45);
    printf("result = %d\n",result);
    return 0;
}