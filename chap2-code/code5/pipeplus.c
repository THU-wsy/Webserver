#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

int main() {
    //父子进程互相发送数据并输出

    //在fork之前创建管道

    int pipefd[2];
    int ret = pipe(pipefd);

    if (ret == -1) {
        perror("pipe");
        exit(0);
    }

    //创建子进程

    pid_t pid = fork();
    
    if (pid > 0) {
        //从管道的读端读数据
        printf("i am parent process, pid : %d\n", getpid());
        char buf[1024] = {0};
        while (1) { 
            // 不断读数据
            int len = read(pipefd[0], buf, sizeof(buf));
            printf("parent recv : %s, pid : %d\n", buf, getpid());
            // 不断写数据
            char* str = "hello, i am a parent!";
            write(pipefd[1], str, strlen(str));
            sleep(1);
        }
    } else if (pid == 0) {
        //从管道的写端写数据
        printf("i am child process, pid : %d\n", getpid());
        char buf[1024] = {0};
        while (1) { 
            // 不断写数据
            char* str = "hello, i am a child!";
            write(pipefd[1], str, strlen(str));
            sleep(1);
            //不断读数据
            int len = read(pipefd[0], buf, sizeof(buf));
            printf("child recv : %s, pid : %d\n", buf, getpid());
        }
    }

    return 0;
}