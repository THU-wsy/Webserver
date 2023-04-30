#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
//利用SIGCHLD处理僵尸进程
void func(int num) {
    printf("捕捉到信号%d\n", num);
    while (1) {
        int ret = waitpid(-1, NULL, WNOHANG);
        if (ret > 0)
            printf("child die, pid = %d\n", ret);
        else break;
    }
}
int main() {
    //提前设置好阻塞SIGCHLD信号，以防止子进程已结束而信号捕捉函数还未设置的情况
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    pid_t pid;
    for (int i = 0; i < 20; i++) {
        pid = fork();
        if (pid == 0)
            break;
    }
    if (pid > 0) {
        signal(SIGCHLD, func);
        //设置完信号捕捉函数以后，解除阻塞
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        while (1) {
            printf("parent process pid : %d\n", getpid());
            sleep(2);
        }
    } else if (pid == 0) {
        printf("child process pid : %d\n", getpid());
    }
    return 0;
}