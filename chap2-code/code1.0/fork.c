#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    //创建子进程
    pid_t pid = fork();
    int num = 0;

    //判断父子进程
    if (pid > 0) {
        printf("The return value of fork is %d\n", pid);
        //当前是父进程
        printf("I am parent process, pid : %d, ppid : %d\n", getpid(), getppid());
        num += 10;
        printf("parent num = %d\n", num);
    } else if (pid == 0) {
        //当前是子进程
        printf("I am child process, pid : %d, ppid : %d\n", getpid(), getppid());
        num += 10;
        printf("child num = %d\n", num);
    }

    for (int i = 0; i < 5; i++) {
        printf("i = %d, pid : %d\n", i, getpid());
        sleep(1);
    }
    return 0;
}
