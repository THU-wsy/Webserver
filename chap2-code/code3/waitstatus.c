#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {

    //有一个父进程，创建5个子进程(兄弟)

    pid_t pid;

    for (int i = 0; i < 5; i++) {
        pid = fork();
        if (pid == 0)
            break;
    }

    if (pid > 0) {
        while (1) {
            printf("parent, pid = %d\n", getpid());
            
            int st;
            int ret = wait(&st);

            if (ret == -1)
                break;
            
            if (WIFEXITED(st)) {
                //是否正常退出
                printf("正常退出，状态码：%d\n", WEXITSTATUS(st));
            }
            if (WIFSIGNALED(st)) {
                //是否异常终止
                printf("被信号 %d 终止\n", WTERMSIG(st));
            }

            printf("child die, pid = %d\n", ret);
            sleep(1);
        }
    } else if (pid == 0) {
        printf("child, pid = %d\n", getpid());
        sleep(3);
        exit(1);
    }

    return 0;
}