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

    // 服务器的地址信息
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    inet_pton(AF_INET, "172.23.55.106", &server_addr.sin_addr.s_addr);

    // 2.通信
    int i = 0;
    while (1) {
        char sendbuf[1024] = {0};
        sprintf(sendbuf, "Hello, I am client, i = %d\n", i++);
        sendto(fd, sendbuf, strlen(sendbuf) + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        struct sockaddr_in client_addr;
        int len = sizeof(client_addr);

        recvfrom(fd, sendbuf, sizeof(sendbuf), 0, NULL, NULL);

        printf("Server say : %s\n", sendbuf);
        sleep(1);
    }

    close(fd);
    return 0;
}