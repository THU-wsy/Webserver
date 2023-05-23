#include <stdio.h>
#include <arpa/inet.h>

int main() {

    unsigned short a = 0x0102;
    printf("a = 0x%x\n", a);
    unsigned short b = htons(a);
    printf("b = 0x%x\n", b);

    unsigned char buf[4] = {192, 168, 1, 100};
    printf("本机字节序IP：%d.%d.%d.%d\n", buf[0], buf[1], buf[2], buf[3]);
    int tmp = *(int*)buf;
    unsigned int newtmp = htonl(tmp);
    unsigned char* p = (char*) &newtmp;
    printf("网络字节序IP：%d.%d.%d.%d\n", *p, *(p + 1), *(p + 2), *(p + 3));

    return 0;
}