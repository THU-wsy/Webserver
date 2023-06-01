// TCP 通信的服务器端
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define maxnum 128 //可同时连接的最大客户端数量

struct sockInfo {
    int fd; // 通信的文件描述符
    struct sockaddr_in addr; 
    pthread_t tid; //线程号
};

struct sockInfo sockInfos[maxnum];

void* working(void* arg) {
    struct sockInfo* infoptr = (struct sockInfo*) arg;
    // 输出客户端的信息
    char clientIP[16];
    inet_ntop(AF_INET, &infoptr->addr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(infoptr->addr.sin_port);
    printf("Client IP is %s, port is %d\n", clientIP, clientPort);

    // 5. 通信
    char recvBuf[1024] = {0};
    while (1) {
        // 获取客户端的数据
        int num = read(infoptr->fd, recvBuf, sizeof(recvBuf));
        if (num == -1) {
            perror("read");
            exit(-1);
        } else if (num > 0) 
            printf("Server receives client's data : %s\n", recvBuf);
        else if (num == 0) {
            //表示客户端断开连接
            printf("Client closed!\n");
            break;
        }
        
        // 给客户端发送数据
        char* data = "Hello, I am server!";
        write(infoptr->fd, data, strlen(data) + 1);
    }
    close(infoptr->fd);

    return NULL;
}

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

    // 初始化数据
    for (int i = 0; i < maxnum; i++) {
        bzero(&sockInfos[i], sizeof(sockInfos[i]));
        sockInfos[i].fd = -1;
        sockInfos[i].tid = -1;
    }

    // 4.不断循环，等待客户端连接
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int fd_client = accept(fd_listen, (struct sockaddr*) &client_addr, &len);
        if (fd_client == -1) {
            perror("accept");
            exit(-1);
        }

        //每一个连接进来，创建一个子线程跟客户端通信
        struct sockInfo* infoptr;
        for (int i = 0; i < maxnum; i++) {
            //找到可以使用的数据块
            if (sockInfos[i].fd == -1) {
                infoptr = &sockInfos[i];
                break;
            }
            if (i == maxnum - 1) {
                printf("连接数过多\n");
                exit(-1);
            }
        }
        infoptr->fd = fd_client;
        memcpy(&infoptr->addr, &client_addr, len);
        pthread_create(&infoptr->tid, NULL, working, infoptr);
        pthread_detach(infoptr->tid);
    }
    

    // 关闭文件描述符

    close(fd_listen);

    return 0;
}