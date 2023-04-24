#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

/*
    设置管道非阻塞
    int flags = fcntl(fd[0], F_GETFL); //获取原来的flag
    flags |= O_NONBLOCK; //修改flag的值，设置非阻塞
    fcntl(fd[0], F_SETFL, flags); //设置新的flag
*/
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
        printf("i am parent process, pid = %d\n", getpid());
        close(pipefd[1]);
        
        //从管道的读端读数据
        char buf[1024] = {0};
        int flags = fcntl(pipefd[0], F_GETFL); //获取原来的flag
        flags |= O_NONBLOCK; //修改flag的值，设置非阻塞
        fcntl(pipefd[0], F_SETFL, flags); //设置新的flag
        
        while (1) {
            int len = read(pipefd[0], buf, sizeof(buf));
            printf("len = %d\n", len);
            printf("parent recv : %s, pid : %d\n", buf, getpid());
            memset(buf, 0, 1024);
            sleep(1);
        }
    } else if (pid == 0) {
        //从管道的写端写数据
        printf("i am child process, pid = %d\n", getpid());
        close(pipefd[0]);
        
        while (1) {
            char* str = "hello, i am a child!";
            write(pipefd[1], str, strlen(str));
            sleep(5);
        }
    }

    return 0;
}