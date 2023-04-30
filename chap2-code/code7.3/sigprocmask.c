//编写一个程序，把所有常规信号(1-31)的未决状态打印到屏幕
//设置某些信号是阻塞的，通过键盘产生这些信号

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {

    //设置2、3号信号阻塞
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, 2);
    sigaddset(&set, 3);

    //修改内核中的阻塞信号集
    sigprocmask(SIG_BLOCK, &set, NULL);

    int num = 0; //用于解除阻塞
    while (1) {
        //获取当前的未决信号集
        sigset_t pendingset;
        sigemptyset(&pendingset);
        sigpending(&pendingset);

        //遍历前31位
        for (int i = 1; i <= 31; i++) {
            printf("%d", sigismember(&pendingset, i));
        }
        printf("\n");
        sleep(1);

        if (++num == 10)
            sigprocmask(SIG_UNBLOCK, &set, NULL);
    }


    return 0;
}