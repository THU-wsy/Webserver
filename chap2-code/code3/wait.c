#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

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
            int ret = wait(NULL);
            if (ret == -1)
                break;
            printf("child die, pid = %d\n", ret);
            sleep(1);
        }
    } else if (pid == 0) {
        while (1) {
            printf("child, pid = %d\n", getpid());
            sleep(10);
        }
    }

    return 0;
}