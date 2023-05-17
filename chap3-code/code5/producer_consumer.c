#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex;
pthread_cond_t cond_fill; //表示缓冲区满
pthread_cond_t cond_empty; //表示缓冲区空
struct Node {
    int val;
    struct Node* next;
};
struct Node* head = NULL;
int size = 0; //缓冲区大小，假定最大为10

void* producer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (size >= 10) 
            pthread_cond_wait(&cond_fill, &mutex);
        struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
        newNode->next = head;
        head = newNode;
        newNode->val = rand() % 1000;
        size++;
        printf("%ld : Add newNode, val = %d, bufsize = %d\n", pthread_self(), newNode->val, size);
        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
void* consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (size == 0) 
            pthread_cond_wait(&cond_empty, &mutex);
        struct Node* tmp = head;
        head = head->next;
        size--;
        printf("%ld : Delete Node, val = %d, bufsize = %d\n", pthread_self(), tmp->val, size);
        free(tmp);
        pthread_cond_signal(&cond_fill);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_fill, NULL);
    pthread_cond_init(&cond_empty, NULL);
    // 5个生产者，5个消费者
    pthread_t ptids[5], ctids[5];
    for (int i = 0; i < 5; i++) {
        pthread_create(&ptids[i], NULL, producer, NULL);
        pthread_create(&ctids[i], NULL, consumer, NULL);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(ptids[i], NULL);
        pthread_join(ctids[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_fill);
    pthread_cond_destroy(&cond_empty);
    pthread_exit(NULL);
    return 0;
}