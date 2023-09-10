#include <stdio.h>
#include <stdlib.h>

int fib(int val) {
    if (val<=2) {
        return 1;
    } else {
        return fib(val - 1) + fib(val - 2);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int val = atoi(argv[1]);  

    if (val<=0) {
        printf("Please enter a positive integer .\n");
        return 1;
    }

    int result = fib(val);
    printf("result = %d\n",result);
    return 0;
}