#include <stdio.h>
#include <stdlib.h>
#include "head.h"

int main(int argc, char* argv[]) {
    int a, b;
    printf("argc = %d\n", argc);

    if (argc < 3) {
        a = 10;
        b = 30;
    } else {
        a = atoi(argv[1]);
        b = atoi(argv[2]);
    }
    printf("a = %d, b = %d\n", a, b);
    printf("a + b = %d\n", a + b);

    for (int i = 0; i < a; ++i) {
        printf("i = %d\n", i);
        // 函数调用
        int res = test(i);
        printf("res value: %d\n", res);
    }
    
    printf("THE END !!!\n");
    return 0;
}