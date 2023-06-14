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

    // 2.绑定
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = 0; //0.0.0.0
    int ret = bind(fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 3.通信
    while (1) {
        char recvbuf[1024] = {0};
        char ip[16];

        struct sockaddr_in client_addr;
        int len = sizeof(client_addr);

        int num = recvfrom(fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*) &client_addr, &len);

        printf("Client IP : %s, Port : %d\n",
            inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
            ntohs(client_addr.sin_port)
        );

        printf("Client say : %s\n", recvbuf);

        sendto(fd, recvbuf, strlen(recvbuf) + 1, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    }

    close(fd);
    return 0;
}