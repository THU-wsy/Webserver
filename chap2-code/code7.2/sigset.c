#include <signal.h>
#include <stdio.h>

int main() {

    //创建一个信号集
    sigset_t set;

    sigemptyset(&set);

    sigaddset(&set, SIGINT);

    int ret = sigismember(&set, SIGINT);
    if (ret == 1) 
        printf("SIGINT阻塞\n");
    else if (ret == 0)
        printf("SIGINT不阻塞\n");

    sigdelset(&set, SIGINT);
    ret = sigismember(&set, SIGINT);
    if (ret == 1) 
        printf("SIGINT阻塞\n");
    else if (ret == 0)
        printf("SIGINT不阻塞\n");

    return 0;
}