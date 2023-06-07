// TCP 通信的服务器端
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

int main() {
    
    // 1.创建socket(用于监听的套接字)
    int fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_listen == -1) {
        perror("socket");
        exit(-1);
    }

    // 2.绑定
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = 0; //0.0.0.0
    int ret = bind(fd_listen, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 3.监听
    ret = listen(fd_listen, 8);
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }

    // 创建一个fd_set的集合，存放的是需要检测的文件描述符
    fd_set rdset, tmp;
    FD_ZERO(&rdset);
    FD_SET(fd_listen, &rdset);
    int maxfd = fd_listen;

    char recvBuf[1024] = {0}; 
    while (1) {
        tmp = rdset;
        // 调用select系统函数，让内核帮检测哪些文件描述符有数据
        ret = select(maxfd + 1, &tmp, NULL, NULL, NULL);
        if (ret == -1) {
            perror("select");
            exit(-1);
        } else if (ret == 0)
            continue;
        else if (ret > 0) {
            // 说明检测到了有文件描述符的对应的缓冲区的数据发生了改变
            if (FD_ISSET(fd_listen, &tmp)) {
                // 表示有新的客户端连接进来了
                // 4.接收客户端连接
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int fd_client = accept(fd_listen, (struct sockaddr*) &client_addr, &len);
                if (fd_client == -1) {
                    perror("accept");
                    exit(-1);
                }

                // 输出客户端的信息
                char clientIP[16];
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, clientIP, sizeof(clientIP));
                unsigned short clientPort = ntohs(client_addr.sin_port);
                printf("Client IP is %s, port is %d\n", clientIP, clientPort);

                // 将新的文件描述符加入到集合中
                FD_SET(fd_client, &rdset);
                // 更新最大的文件描述符
                maxfd = maxfd > fd_client ? maxfd : fd_client;
            }

            for (int i = fd_listen + 1; i <= maxfd; i++)
                if (FD_ISSET(i, &tmp)) {
                    // 说明这个文件描述符对应的客户端发来了数据, 进行通信
                    int num = read(i, recvBuf, sizeof(recvBuf));
                    if (num == -1) {
                        perror("read");
                        exit(-1);
                    } else if (num > 0) {
                        printf("Server receives client's data : %s\n", recvBuf);
                        write(i, recvBuf, strlen(recvBuf) + 1);
                    } else if (num == 0) {
                        //表示客户端断开连接
                        printf("Client closed!\n");
                        close(i);
                        FD_CLR(i, &rdset);
                    }
                }
        }
    }

    // 关闭文件描述符
    close(fd_listen);

    return 0;
}