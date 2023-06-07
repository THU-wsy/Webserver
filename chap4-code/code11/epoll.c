// TCP 通信的服务器端
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>

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

    // 调用epoll_create()创建一个epoll实例
    int epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create");
        exit(-1);
    }

    // 将监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = fd_listen;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd_listen, &epev);
    if (ret == -1) {
        perror("epoll_ctl");
        exit(-1);
    }

    struct epoll_event epevs[1024];

    char recvBuf[1024] = {0}; 
    while (1) {
        
        int number = epoll_wait(epfd, epevs, 1024, -1);
        if (number == -1) {
            perror("epoll_wait");
            exit(-1);
        } else if (number == 0)
            continue;
        
        for (int i = 0; i < number; i++) {
            int fdi = epevs[i].data.fd;
            if (fdi == fd_listen) {
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
                
                epev.events = EPOLLIN;
                epev.data.fd = fd_client;
                epoll_ctl(epfd, EPOLL_CTL_ADD, fd_client, &epev);
            } else {
                if (!(epevs[i].events & EPOLLIN))
                    continue;
                // 有数据到达，需要通信
                int num = read(fdi, recvBuf, sizeof(recvBuf));
                if (num == -1) {
                    perror("read");
                    exit(-1);
                } else if (num > 0) {
                    printf("Server receives client's data : %s\n", recvBuf);
                    write(fdi, recvBuf, strlen(recvBuf) + 1);
                } else if (num == 0) {
                    //表示客户端断开连接
                    printf("Client closed!\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fdi, NULL);
                    close(fdi);
                }
            }
        }      
    }

    // 关闭文件描述符
    close(fd_listen);
    close(epfd);
    return 0;
}