#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

int main() {
    //子进程发送数据给父进程，父进程读取到数据后输出

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
        char buf[1024] = {0};
        int len = read(pipefd[0], buf, sizeof(buf));
        printf("parent recv : %s, pid : %d\n", buf, getpid());
    } else if (pid == 0) {
        //从管道的写端写数据
        sleep(5);
        char* str = "hello, i am a child!";
        write(pipefd[1], str, strlen(str));
    }

    return 0;
}