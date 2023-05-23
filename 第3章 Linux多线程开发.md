# 1. 创建线程

```c
#include <pthread.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                void *(*start_routine) (void *), void *arg);
```
- 作用：创建一个子线程(一般情况下，main函数所在的线程称为主线程，其余创建的线程称为子线程)
- 参数：
  - thread：传出参数，指向子线程的线程ID
  - attr：设置线程的属性，一般传入NULL表示使用默认值
  - start_routine：函数指针，这个函数是子线程需要处理的逻辑代码
  - arg：给函数start_routine传递的参数
- 返回值：成功返回0，失败则返回错误号(与之前的errno不同，该错误号可以通过char \* strerror(int errnum)来获取错误信息)

```c
#include <pthread.h>

pthread_t pthread_self(void);
```
- 作用：获取当前线程的线程号

```c
#include <pthread.h>

int pthread_equal(pthread_t t1, pthread_t t2);
```
- 作用：判断两个线程号是否相等(注意一般来说pthread_t是无符号长整型，但也有的系统是用结构体去实现的)
- 返回值：相等则返回非0值，不相等则返回0

# 2. 线程退出和回收

```c
#include <pthread.h>

void pthread_exit(void *retval);
```
- 作用：退出当前线程(注意如果退出了主线程，它并不会影响其他正在运行的线程)
- 参数：retval是一个指针，作为一个返回值，可以在pthread_join()中获取

```c
#include <pthread.h>

int pthread_join(pthread_t thread, void **retval);
```
- 作用：回收一个已经退出的子线程的资源，该函数是阻塞的，调用一次只能回收一个子线程，一般在主线程中调用该函数
- 参数：thread是要回收的子线程id；retval用于接收子线程退出时的返回值
- 返回值：成功则返回0，失败则返回错误号

```c
#include <pthread.h>

int pthread_detach(pthread_t thread);
```
- 作用：分离一个线程thread，被分离的线程在退出时会自动释放资源给系统。注意不能多次分离同一线程，且不能join一个已经分离的线程。
- 返回值：成功返回0，失败返回错误号

```c
#include <pthread.h>

int pthread_cancel(pthread_t thread);
```
- 作用：取消线程(让线程退出)。注意调用该函数后并不是立即终止线程，而是当该线程执行到一个取消点后，线程才会终止。(取消点是系统规定好的一些系统调用)


# 3. 线程属性

```c
#include <pthread.h>

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
```
- 作用：分别是初始化线程属性变量、释放线程属性的资源、获取线程分离的状态属性、设置线程分离的状态属性

# 4. 锁

## 4.1 互斥锁

```c
#include <pthread.h>
pthread_mutex_t //互斥量的类型

int pthread_mutex_init(pthread_mutex_t *restrict mutex,
           const pthread_mutexattr_t *restrict attr);
```
- 作用：初始化互斥量
- 参数：mutex是要初始化的互斥量变量；attr是互斥量相关的属性，一般传入默认参数NULL即可(restrict是C语言的修饰符，被修饰的指针不能由另一个指针进行操作)

```c
#include <pthread.h>

int pthread_mutex_destroy(pthread_mutex_t *mutex);
```
- 作用：释放互斥量的资源

```c
#include <pthread.h>

int pthread_mutex_lock(pthread_mutex_t *mutex);
```
- 作用：加锁。该函数是阻塞的(如果有一个线程已经对该互斥量加锁了，那么其他线程调用该函数将阻塞等待)。

```c
#include <pthread.h>

int pthread_mutex_trylock(pthread_mutex_t *mutex);
```
- 作用：尝试加锁，如果加锁失败则直接返回

```c
#include <pthread.h>

int pthread_mutex_unlock(pthread_mutex_t *mutex);
```
- 作用：解锁


## 4.2 读写锁

读写锁的特点：
- 如果有线程读数据，则允许其他线程执行读操作，但不允许写操作。
- 如果有线程写数据，则其他线程都不允许读、写操作。
- 写是独占的，写的优先级高。

```c
#include <pthread.h>
pthread_rwlock_t //读写锁的类型

int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
           const pthread_rwlockattr_t *restrict attr);       
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

# 5. 条件变量

```c
#include <pthread.h>
pthread_cond_t //条件变量的类型
       
int pthread_cond_init(pthread_cond_t *restrict cond,
           const pthread_condattr_t *restrict attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *restrict cond,
           pthread_mutex_t *restrict mutex);
int pthread_cond_timedwait(pthread_cond_t *restrict cond,
           pthread_mutex_t *restrict mutex,
           const struct timespec *restrict abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
```

# 6. 信号量

```c
#include <semaphore.h>
sem_t //信号量的类型

int sem_init(sem_t *sem, int pshared, unsigned int value);
```
- 作用：初始化信号量
- 参数：sem是信号量的地址；pshared为0表示信号量用于线程间，不为0表示用于进程间；value表示要初始化的信号量的值

```c
#include <semaphore.h>

int sem_destroy(sem_t *sem);
```
- 作用：释放信号量资源

```c
#include <semaphore.h>

int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
```
- 作用：如果sem的值大于0，则调用wait后会让sem的值减一，然后立刻返回；如果sem的值等于0，则调用wait后会阻塞，直到sem的值大于0后再减一并返回。

```c
#include <semaphore.h>

int sem_post(sem_t *sem);
```
- 作用：将sem的值加1，然后唤醒一个睡眠的线程。

```c
#include <semaphore.h>

int sem_getvalue(sem_t *sem, int *sval);
```
- 作用：获取信号量的值

生产者消费者问题(互斥锁+条件变量实现)：
```c
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
```

生产者消费者问题(信号量实现)：
```c
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
```