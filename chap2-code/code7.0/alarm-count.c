//计算机1秒能打印多少个数
#include <unistd.h>
#include <stdio.h>

int main() {
    alarm(1);
    int i = 0;
    while (++i)
        printf("%d\n", i);

    return 0;
}