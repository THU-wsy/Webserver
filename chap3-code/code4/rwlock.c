/*
8个线程，其中3个写变量，5个读变量。
*/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int num = 1;
pthread_rwlock_t rwlock;

void* writeNum(void* arg) {

    while (1) {
        pthread_rwlock_wrlock(&rwlock);
        num++;
        printf("%ld : write, num = %d\n", pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(1000);
    }
    return NULL;
}

void* readNum(void* arg) {

    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("%ld : read, num = %d\n", pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(1000);
    }
    return NULL;
}


int main() {

    pthread_rwlock_init(&rwlock, NULL);
    pthread_t wtids[3], rtids[5];
    for (int i = 0; i < 3; i++)
        pthread_create(&wtids[i], NULL, writeNum, NULL);
    for (int i = 0; i < 5; i++)
        pthread_create(&rtids[i], NULL, readNum, NULL);

    for (int i = 0; i < 3; i++)
        pthread_join(wtids[i], NULL);
    for (int i = 0; i < 5; i++)
        pthread_join(rtids[i], NULL);
    
    pthread_rwlock_destroy(&rwlock);
    pthread_exit(NULL);

    return 0;
}