#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void* callback1(void* arg) {
    printf("child thread1...\n");
    return NULL;
}

void* callback2(void* arg) {
    printf("child thread2...it has an arg\n");
    printf("arg num = %d\n", *(int*) arg);
    return NULL;
}

int main() {

    int num = 666;
    // 创建一个子线程
    pthread_t tid;
    int ret1 = pthread_create(&tid, NULL, callback1, NULL);
    int ret2 = pthread_create(&tid, NULL, callback2, (void*) &num);

    if (ret1 != 0) {
        char* errstr = strerror(ret1);
        printf("error : %s\n", errstr);
    }

    if (ret2 != 0) {
        char* errstr = strerror(ret2);
        printf("error : %s\n", errstr);
    }

    for (int i = 0; i < 5; i++) 
        printf("%d\n", i);

    sleep(1);
    return 0;
}