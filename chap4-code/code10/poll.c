// TCP 通信的服务器端
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <poll.h>

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

    // 创建一个pollfd数组
    struct pollfd fds[1024];
    for (int i = 0; i < 1024; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }
    fds[0].fd = fd_listen;

    int nfds = 0;

    char recvBuf[1024] = {0}; 
    while (1) {
        // 调用poll系统函数，让内核帮检测哪些文件描述符有数据
        ret = poll(fds, nfds + 1, -1);
        if (ret == -1) {
            perror("poll");
            exit(-1);
        } else if (ret == 0)
            continue;
        else if (ret > 0) {
            // 说明检测到了有文件描述符的对应的缓冲区的数据发生了改变
            if (fds[0].revents & POLLIN) {
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

                // 将新的文件描述符加入到数组中
                for (int i = 1; i < 1024; i++)
                    if (fds[i].fd == -1) {
                        fds[i].fd = fd_client;
                        fds[i].events = POLLIN;
                        fds[i].revents = 0;
                        nfds = nfds > i ? nfds : i;
                        break;
                    }
            }

            for (int i = 1; i <= nfds; i++)
                if (fds[i].revents & POLLIN) {
                    // 说明这个文件描述符对应的客户端发来了数据, 进行通信
                    int num = read(fds[i].fd, recvBuf, sizeof(recvBuf));
                    if (num == -1) {
                        perror("read");
                        exit(-1);
                    } else if (num > 0) {
                        printf("Server receives client's data : %s\n", recvBuf);
                        write(fds[i].fd, recvBuf, strlen(recvBuf) + 1);
                    } else if (num == 0) {
                        //表示客户端断开连接
                        printf("Client closed!\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        fds[i].events = POLLIN;
                        fds[i].revents = 0;
                    }
                }
        }
    }

    // 关闭文件描述符
    close(fd_listen);

    return 0;
}