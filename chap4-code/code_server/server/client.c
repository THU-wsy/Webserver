// TCP 通信的客户端
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char IP[16] = {0};

int main() {

    // 1.创建socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    // 2.连接服务器端
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    printf("请输入要连接的服务器的IP地址：\n");
    scanf("%s", IP);
    inet_pton(AF_INET, IP, &server_addr.sin_addr.s_addr);
    int ret = connect(fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("connect");
        exit(-1);
    }

    // 3. 通信
    char recvBuf[1024] = {0};
    while(1) {
        // 给服务器发送数据
        char * data = "Hello, I am client!";
        write(fd, data , strlen(data));
        sleep(1);
        // 读取服务器发送过来的数据
        int num = read(fd, recvBuf, sizeof(recvBuf));
        if(num == -1) {
            perror("read");
            exit(-1);
        } else if(num > 0) {
            printf("Client receives server's data : %s\n", recvBuf);
        } else if(num == 0) {
            // 表示服务器端断开连接
            printf("Server closed!\n");
            break;
        }
    }

    // 关闭文件描述符
    close(fd);

    return 0;
}