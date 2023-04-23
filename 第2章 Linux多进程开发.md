# 1. 进程创建fork()

## 1.1 进程基本操作

- 查看进程
```shell
ps aux / ajx
```

![](zzimages/20230419201026.png)

- 实时显示进程的动态
```shell
top
```

- 杀死进程

```shell
kill [-signal] pid
kill -l //列出所有信号
killall name //根据进程名杀死进程
```
例如以下命令可以强制杀死进程
```shell
kill -SIGKILL(或-9) 进程ID
```

## 1.2 进程号

每个进程都由进程号来标识，其类型为pid_t，进程号的范围是0~32767。

任何进程(除init进程)都是由另一个进程创建，该进程被称为被创建进程的父进程，对应的进程号称为父进程号(PPID)。

进程组是一个或多个进程的集合，它们之间相互关联，进程组可以接收同一终端的各种信号，关联的进程有一个进程组号(PGID)。默认情况下，当前的进程号会当作当前的进程组号。

相关函数：
```c
pid_t getpid(void);
pid_t getppid(void);
pid_t getpgid(pid_t pid);
```

## 1.3 进程创建fork()

系统允许一个进程创建新的子进程，从而形成进程树结构模型。

```c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
```
- 返回值：==成功则在子进程中返回0，在父进程中返回子进程的进程ID==；失败返回-1，失败主要有两个原因，一是当前系统的进程数已经达到了系统规定的上限，此时errno值被设置为EAGAIN，二是系统内存不足，此时errno值被设置为ENOMEM
- 作用：系统调用fork()用于创建子进程(==一个完全相同的进程，复制父进程的地址空间，只有PCB中的一些数据如PID、PPID以及fork()的返回值不同==)。子进程不会从main()函数开始执行，而是直接从fork()系统调用返回后继续执行。

事实上，更准确来说，fork()是通过==写时拷贝==(copy-on-write)来实现的。写时拷贝是一种可以推迟甚至避免拷贝数据的技术。调用fork()时内核并不复制整个进程的地址空间，而是让父子进程共享同一个地址空间，只有在需要写入的时候才会复制地址空间，从而使各个进程拥有各自的地址空间。即，资源的复制是在需要写入的时候才会进行，在此之前，以只读方式共享。注意，fork之后父子进程共享文件，父子进程相同的文件描述符指向相同的文件表，引用计数增加，共享文件偏移指针。

## 1.4 GDB多进程调试

使用GDB调试的时候，GDB默认只能跟踪一个进程，可以在fork函数调用之前，通过指令设置GDB调试工具跟踪父进程或者是跟踪子进程，默认跟踪父进程。

- 设置调试父进程或者子进程

```shell
set follow-fork-mode parent(默认，或者child)
```

- 设置调试模式

```shell
set detach-on-fork on(或者off)
```
默认为on，表示调试当前进程的时候，其他的进程继续运行。如果为off，则调试当前进程的时候，其他进程被GDB挂起。

- 查看调试的进程

```shell
info inferiors
```

- 切换当前调试的进程

```shell
inferior id
```

- 使进程脱离GDB调试

```shell
detach inferiors id
```

# 2. exec函数族

exec函数族的作用是根据指定的文件名找到可执行文件，并用它来取代调用进程的内容，换句话说，就是在调用进程内部执行一个可执行文件。exec函数族的函数执行成功后不会返回，因为调用进程的用户区(代码段、数据段、堆、栈等)已经被新内容取代，只留下进程ID等一些表面上的信息仍保持原样；只有调用失败后才会返回-1，并从原程序的调用点接着往下执行。

==一般先创建一个子进程，然后在子进程中执行exec函数族中的函数。==

```c
#include <unistd.h>
int execl(const char *pathname, const char *arg, ...
            /* (char  *) NULL */);
int execlp(const char *file, const char *arg, ...
            /* (char  *) NULL */);
int execle(const char *pathname, const char *arg, ...
            /*, (char *) NULL, char *const envp[] */);
int execv(const char *pathname, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[],
            char *const envp[]);
int execve(const char *pathname, char *const argv[],
            char *const envp[]);
```
前6个函数是标准C库的函数，最后一个是UNIX系统调用。

|字母|含义|
|---|---|
|l(list)|参数地址列表，以空指针结尾|
|v(vector)|存有各参数地址的指针数组的地址|
|p(path)|按PATH环境变量指定的目录搜索可执行文件|
|e(environment)|存有环境变量字符串地址的指针数组的地址|

## 2.1 execl()

```c
int execl(const char *pathname, const char *arg, ...
            /* (char  *) NULL */);
```
- 参数：pathname需要指定的可执行文件的路径；arg是执行可执行文件所需要的参数列表，第一个参数一般没什么作用，通常都写的是可执行程序的名称，第二个参数开始就是所需的参数列表，参数最后需要以NULL结束。
- 返回值：只有调用失败时才会返回，返回-1，并设置errno

## 2.2 execlp()

```c
int execlp(const char *file, const char *arg, ...
            /* (char  *) NULL */);
```
- 参数：file是需要指定的可执行文件的文件名。execlp会到环境变量中查找指定的可执行文件，如果找了就执行，找不到则执行失败。

# 3. 进程退出和回收

## 3.1 exit()

```c
#include <stdlib.h>
void exit(int status); //标准C库
#include <unistd.h>
void _exit(int status); //Linux系统调用
```

![](zzimages/20230422161226.png)

- 参数：status是进程退出时的一个状态信息，父进程回收子进程资源的时候可以获取到。

示例：
```c
    printf("hello\n");
    printf("world");
    exit(0);
```
将输出
```shell
hello
world
```
然而
```c
    printf("hello\n");
    printf("world");
    _exit(0);
```
将输出
```shell
hello
```
因为调用_exit不会刷新缓冲区，所以"world"仍在缓冲区内。

## 3.2 孤儿进程

父进程运行结束，但子进程还未运行结束，这样的子进程就称为孤儿(orphan)进程。==每当出现一个孤儿进程时，内核就把孤儿进程的父进程设置为init(进程ID为1)，而init进程会循环地wait()它的已经退出的子进程==。这样，当一个孤儿进程结束生命周期时，init进程就会处理善后工作，回收其资源。所以孤儿进程不会有什么危害。

## 3.3 僵尸进程

每个进程结束之后，都会释放自己地址空间中的用户区数据，内核区的PCB没有办法自己释放掉，需要父进程去释放。进程终止时，父进程尚未回收，子进程残留资源(PCB)存放于内核中，变成僵尸(zombie)进程。僵尸进程不能被kill -9杀死，这样就会导致一个问题：==如果父进程不调用wait()或waitpid()的话，那么保留的那段信息就不会释放，其进程号就会一直被占用，但是系统所能使用的进程号是有限的，如果产生大量僵尸进程，就会因为没有可用的进程号而导致系统不能产生新的进程，这就是僵尸进程的危害，应当避免==。

## 3.4 进程回收

在每个进程退出的时候，内核释放该进程所有的资源：包括打开的文件、占用的内存等，但是仍然为其保留一定的信息：主要是PCB的信息(进程号、退出状态、运行时间等)。父进程可以通过调用wait或waitpid得到子进程的退出状态同时彻底清除掉这个子进程。wait()和waitpid()功能一样，区别在于wait()函数会阻塞，而waitpid()可以设置不阻塞，并且waitpid()还可以指定等待哪个子进程结束。注意，一次wait或waitpid调用只能清理一个子进程，清理多个子进程应使用循环。

```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *wstatus);
```
- 作用：等待任意一个子进程结束，如果某一个子进程结束了，此函数会回收子进程的资源。
- 参数：wstatus是一个传出参数(int类型的地址)，指向进程退出时的状态信息。
- 返回值：成功则返回被回收的子进程的pid；失败则返回-1(失败有两种情况：一种是调用wait失败了，另一种是所有的子进程都结束了)

调用wait()函数的进程会被挂起(阻塞)，直到它的一个子进程退出或者收到一个不能忽略的信号时才会被唤醒。如果没有子进程，或是子进程都结束了，则调用wait()会立即返回-1

退出信息相关宏函数：
|宏|作用|
|---|---|
|WIFEXITED(status)|如果返回值非0，表示进程正常退出|
|WEXITSTATUS(status)|如果上面的宏为真，则获取进程退出的状态(即exit的参数)|
|WIFSIGNALED(status)|如果返回值非0，表示进程异常终止|
|WTERMSIG(status)|如果上面的宏为真，则获取使进程终止的信号编号|
|WIFSTOPPED(status)|如果返回值非0，表示进程处于暂停状态|
|WSTOPSIG(status)|如果上面的宏为真，则获取使进程暂停的信号编号|
|WIFCONTINUED(status)|如果返回值非0，表示进程暂停后已经继续运行|


```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *wstatus, int options);
```
- 作用：回收指定进程号的子进程，可以设置是否阻塞。
- 参数：
  - pid：若pid > 0，则表示回收进程号为pid的子进程；若pid = 0，则表示回收当前进程组的任意子进程(注意当前进程的某些子进程可以在别的进程组)；若pid = -1，则表示回收任意子进程，相当于wait；若pid < -1，则表示回收组ID为-pid的进程组中的任意子进程。
  - wstatus与wait()的参数相同
  - options：用于设置阻塞或非阻塞，0表示阻塞，WNOHANG表示非阻塞
- 返回值：
  - 大于0：表示返回被回收的子进程的pid
  - 等于0：此时要求options = WNOHANG，表示还有子进程活着
  - 等于-1：表示调用失败或者所有子进程都结束了

可见wait(&st)等价于waitpid(-1, &st, 0)。


# 4. 进程间通信简介

进程是一个独立的资源分配单元，不同进程(通常指用户进程)之间的资源是独立的，所以不能在一个进程中直接访问另一个进程的资源。但是，不同的进程又需要进行信息的交互和状态的传递等，因此需要进程间通信(IPC: Inter Process Communication)。

![](zzimages/20230423204858.png)

# 5. 管道

管道的特点：
- 管道其实是一个在内核内存中维护的缓冲器，这个缓冲器的存储能力是有限的，不同的操作系统大小不一定相同。
- 管道拥有文件的特质：读操作、写操作，匿名管道没有文件实体，有名管道有文件实体、但不存储数据。可以按照操作文件的方式对管道进行操作。
- 一个管道是一个字节流，使用管道时不存在消息或者消息边界的概念，从管道读取数据的进程可以读取任意大小的数据块，而不管写入进程写入管道的数据块的大小是多少。
- 通过管道传递的数据是顺序的，从管道中读取出来的字节的顺序和它们被写入管道的顺序是完全一样的。
- 在管道中的数据的传递方向是单向的，一端用于写入，一端用于读取，管道是半双工的。
- 从管道读数据是一次性操作，数据一旦被读走，它就从管道中被抛弃，释放空间以便写更多的数据，所以在管道中无法使用lseek()来随机地访问数据。
- ==匿名管道只能在具有公共祖先的进程==(父进程与子进程，或者两个兄弟进程，或者具有亲缘关系的进程)之间使用，这是因为只有fork()出来的进程，它们相同的文件描述符才会指向相同的匿名管道，所以必须在父进程调用fork()之前创建匿名管道。

![](zzimages/20230423212622.png)

![](zzimages/20230423212653.png)

## 5.1 匿名管道

一般将匿名管道简称为管道，它是UNIX系统IPC最古老的形式，所有的UNIX系统都支持这种通信机制。

![](zzimages/20230423212854.png)

```c
#include <unistd.h>

int pipe(int pipefd[2]);
```
- 作用：创建一个匿名管道，用于进程间通信。
- 参数：int pipefd[2]这个数组是一个传出参数，pipefd[0]对应管道的读端，pipefd[1]对应管道的写端。
- 返回值：成功返回0，失败返回-1

注意：管道默认是阻塞的，即如果管道中没有数据，则read阻塞，而如果管道满了，则write阻塞。

```shell
ulimit -a
```
- 作用：查看管道缓冲大小

```c
#include <unistd.h>

long fpathconf(int fd, int name);
```
- 作用：查看管道缓冲大小，用法如下

```c
long size = fpathconf(pipefd[0], _PC_PIPE_BUF);
```