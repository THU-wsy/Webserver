#include <stdio.h>
#include <arpa/inet.h>

int main() {

    char buf[] = "192.168.1.4";
    unsigned int num = 0;
    inet_pton(AF_INET, buf, (void*) &num);
    
    printf("网络字节序整数：%d\n", num);
    unsigned char* p = (unsigned char*) &num;
    printf("其每个字节分别为：%d,%d,%d,%d\n", *p, *(p + 1), *(p + 2), *(p + 3));

    char dst[16];
    inet_ntop(AF_INET, (void*) &num, dst, 16);
    printf("对应的IP地址为：%s\n", dst);

    return 0;
}