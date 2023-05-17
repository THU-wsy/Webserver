#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

sem_t mutex;
sem_t sem_fill; //表示空余缓冲区大小
sem_t sem_empty; //表示已占缓冲区大小
struct Node {
    int val;
    struct Node* next;
};
struct Node* head = NULL;
int size = 0; //缓冲区大小，假定最大为10

void* producer(void* arg) {
    while (1) {
        sem_wait(&sem_fill);
        sem_wait(&mutex);
        struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
        newNode->next = head;
        head = newNode;
        newNode->val = rand() % 1000;
        size++;
        printf("%ld : Add newNode, val = %d, bufsize = %d\n", pthread_self(), newNode->val, size);
        sem_post(&mutex);
        sem_post(&sem_empty);
    }
    return NULL;
}
void* consumer(void* arg) {
    while (1) {
        sem_wait(&sem_empty);
        sem_wait(&mutex);
        struct Node* tmp = head;
        head = head->next;
        size--;
        printf("%ld : Delete Node, val = %d, bufsize = %d\n", pthread_self(), tmp->val, size);
        free(tmp);
        sem_post(&mutex);
        sem_post(&sem_fill);
    }
    return NULL;
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&sem_fill, 0, 10);
    sem_init(&sem_empty, 0, 0);
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
    sem_destroy(&mutex);
    sem_destroy(&sem_fill);
    sem_destroy(&sem_empty);
    pthread_exit(NULL);
    return 0;
}