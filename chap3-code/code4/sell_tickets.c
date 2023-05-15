/*
用多线程实现自动售票，一共100张票，3个售票窗口
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int tickets = 100;
pthread_mutex_t mutex;

void* sellticket(void* arg) {
    
    while (1) {
        pthread_mutex_lock(&mutex);
        if (tickets > 0) {
            printf("%ld窗口正在卖第%d张门票\n", pthread_self(), tickets);
            tickets--;
        } else {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

int main() {

    pthread_mutex_init(&mutex, NULL);

    //创建3个子线程

    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, sellticket, NULL);
    pthread_create(&tid2, NULL, sellticket, NULL);
    pthread_create(&tid3, NULL, sellticket, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);
    return 0;
}