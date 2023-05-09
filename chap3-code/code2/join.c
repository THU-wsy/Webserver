#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int val = 10;
void* callback(void* arg) {
    printf("child thread id = %ld\n", pthread_self());
    sleep(3);
    pthread_exit(&val);
}

int main() {

    pthread_t tid;
    pthread_create(&tid, NULL, callback, NULL);

    for (int i = 0; i < 4; i++)
        printf("i = %d\n", i);
    
    printf("main thread id = %ld, tid = %ld\n", pthread_self(), tid);

    int* retval;
    pthread_join(tid, (void**) &retval);
    printf("child thread exit val = %d\n", *retval);
    pthread_exit(NULL);

    return 0;
}