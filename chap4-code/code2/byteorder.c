/*
判断本机是大端字节序还是小端字节序
*/

#include <stdio.h>

int main() {

    union {
        short val;
        char num[sizeof(short)];
    } test;

    test.val = 0x0102;
    if (test.num[0] == 2 && test.num[1] == 1)
        printf("本机是小端字节序\n");
    else if (test.num[0] == 1 && test.num[1] == 0)
        printf("本机是大端字节序\n");
    else  
        printf("出现错误\n");

    return 0;
}