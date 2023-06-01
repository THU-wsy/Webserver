// TCP 通信的客户端
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

char IP[16] = {0};
struct sockInfo {
    int fd; // 通信的文件描述符
    struct sockaddr_in addr; 
    pthread_t tid; //线程号
} info;

void* readdata(void* arg) {
    struct sockInfo* infoptr = (struct sockInfo*) arg;
    char recvBuf[1024] = {0};
    while(1) {
        memset(recvBuf, 0, sizeof(recvBuf));
        int num = read(infoptr->fd, recvBuf, sizeof(recvBuf));
        if(num == -1) {
            perror("read");
            exit(-1);
        } else if(num > 0) {
            printf("wsy : %s\n", recvBuf);
        } else if(num == 0) {
            // 表示服务器端断开连接
            printf("Server closed!\n");
            break;
        }
    }

    return NULL;
}

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

    // 5. 通信
    char Buf[1024] = {0};
    printf("注：输入QUIT结束聊天\n");
    //主线程用于写数据，创建一个子线程用于读数据
    struct sockInfo* infoptr = &info;
    infoptr->fd = fd;
    memcpy(&infoptr->addr, &server_addr, sizeof(server_addr));
    pthread_create(&infoptr->tid, NULL, readdata, infoptr);
    pthread_detach(infoptr->tid);
    while (1) {
        scanf("%s", Buf);
        if (Buf[0] == 'Q' && Buf[1] == 'U' && Buf[2] == 'I' && Buf[3] == 'T' && Buf[4] == 0){
            pthread_cancel(infoptr->tid);
            break;
        }
        write(fd, Buf, strlen(Buf));
    }

    // 关闭连接
    close(fd);

    return 0;
}