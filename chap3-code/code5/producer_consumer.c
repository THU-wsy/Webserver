#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

struct Node {
    int val;
    struct Node* next;
};
struct Node* head = NULL;

void* producer(void* arg) {

    while (1) {
        struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
        newNode->next = head;
        head = newNode;
        newNode->val = rand() % 1000;
        printf("%ld : Add newNode, val = %d\n", pthread_self(), newNode->val);
        usleep(1000);
    }

    return NULL;
}
void* consumer(void* arg) {

    while (1) {
        struct Node* tmp = head;
        head = head->next;
        printf("%ld : Delete Node, val = %d\n", pthread_self(), tmp->val);
        free(tmp);
        usleep(1000);
    }
    return NULL;
}

int main() {
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
    pthread_exit(NULL);
    return 0;
}