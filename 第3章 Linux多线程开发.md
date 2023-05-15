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

# 4. 互斥锁

## 4.1 互斥量API

```c
pthread_mutex_t //互斥量的类型
```

```c
#include <pthread.h>

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

