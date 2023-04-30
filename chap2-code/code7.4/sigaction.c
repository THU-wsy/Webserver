#include <sys/time.h>
#include <stdio.h>
#include <signal.h>

void func(int n) {
    printf("捕捉到信号%d，程序继续执行\n", n);
}

//过3秒以后，每隔2秒定时一次
int main() {
    
    //signal(SIGALRM, func);
    struct sigaction act;
    act.sa_handler = func;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);

    sigaction(SIGALRM, &act, NULL);

    struct itimerval new_value;

    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_usec = 0;

    new_value.it_value.tv_sec = 3;
    new_value.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &new_value, NULL);
    printf("定时器开始了!\n");

    while (1);
    
    return 0;
}