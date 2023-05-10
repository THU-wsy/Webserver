/*
用多线程实现自动售票，一共100张票，3个售票窗口
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int tickets = 100;

void* sellticket(void* arg) {
    while (tickets > 0) {
        usleep(5000);
        printf("%ld窗口正在卖第%d张门票\n", pthread_self(), tickets);
        tickets--;
    }
    return NULL;
}

int main() {

    //创建3个子线程

    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, sellticket, NULL);
    pthread_create(&tid2, NULL, sellticket, NULL);
    pthread_create(&tid3, NULL, sellticket, NULL);

    pthread_detach(tid1);
    pthread_detach(tid2);
    pthread_detach(tid3);

    pthread_exit(NULL);
    return 0;
}