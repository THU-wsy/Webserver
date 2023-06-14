#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {

    // 1.创建一个通信的socket
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    // 设置广播属性
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(9999);
    inet_pton(AF_INET, "172.23.63.255", &client_addr.sin_addr.s_addr);

    // 3.通信
    int i = 0;
    while (1) {
        char sendBuf[128];
        sprintf(sendBuf, "Hello client, i = %d\n", i++);
        // 发送数据
        sendto(fd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        printf("广播的数据：%s\n", sendBuf);
        sleep(1);
    }

    close(fd);
    return 0;
}