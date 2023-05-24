# 1. socket简介

socket(套接字)是对网络中不同主机上的应用进程之间进行双向通信的端点的抽象。一个套接字就是网络上进程通信的一端，提供了应用层进程利用网络协议交换数据的机制。从所处的地位来讲，套接字上联应用进程，下联网络协议栈，是应用程序通过网络协议进行通信的接口。

socket可以看作是两个网络应用程序进行通信时，各自通信连接中的端点，是一个逻辑上的概念。它是网络环境中进程间通信的API，也是可以被命名和寻址的通信端点，使用中的每一个套接字都有其类型和一个与之相连的进程。通信时其中一个网络应用程序将要传输的数据写入它所在主机的socket中，该socket通过与网卡相连的传输介质将这些数据送到另外一台主机的socket中，使对方能够接收到这些数据。socket是由IP地址和端口结合的，提供向应用层进程传送数据包的机制。

在Linux环境下，socket用于表示进程间网络通信的特殊文件类型，本质是内核借助缓冲区形成的伪文件。所以我们可以使用文件描述符引用套接字。与管道类似，Linux系统将其封装成文件的目的是为了统一接口，使得读写套接字和读写文件的操作一致。区别是管道主要用于本地进程间通信，而套接字主要用于网络进程间通信。套接字通信分为两端，其中服务器端被动接受连接，一般不会主动发起连接，而客户端则主动向服务器发起连接。

# 2. 字节序

在各种计算机体系结构中，对于字节的存储机制有所不同。字节序，即字节的顺序，就是大于一个字节类型的数据在内存中的存放顺序。分为大端字节序和小端字节序：
- 大端字节序：一个整数的高位字节存储在内存的低地址处，低位字节存储在内存的高地址处。
- 小端字节序：一个整数的高位字节存储在内存的高地址处，低位字节存储在内存的低地址处。

例如从左至右为内存地址增长的方向，那么数0x01020304用大端存储即为0x01020304，而用小端存储即为0x04030201

## 字节序转换函数

当格式化的数据在两台使用不同字节序的主机之间直接传递时，接收端必然错误地解释之。为此，发送端应把要发送的数据转换成大端字节序数据后再发送，而接收端知道对方传送过来的数据总是采用大端字节序，所以接收端可以根据自身采用的字节序决定是否对接收到的数据进行转换。

网络字节顺序是TCP/IP中规定好的一种数据表示格式，它与具体的CPU类型、操作系统等无关，从而可以保证数据在不同主机之间传输时能够被正确解释，网络字节顺序采用大端字节序。

BSD Socket提供了封装好的转换接口，包括从主机字节序到网络字节序的转换函数：htons、htonl，以及从网络字节序到主机字节序的转换函数：ntohs、ntohl。其中h表示host(主机)，n表示network(网络)。

```c
#include <arpa/inet.h>
//以下两个函数一般用于转换端口
uint16_t htons(uint16_t hostshort);
uint16_t ntohs(uint16_t netshort);
//以下两个函数一般用于转换IP
uint32_t htonl(uint32_t hostlong);
uint32_t ntohl(uint32_t netlong);
```

# 3. socket地址

socket地址是一个结构体，封装端口号和IP等信息。与socket相关的API会用到这个socket地址。

## 通用socket地址

socket网络编程接口中表示socket地址的是结构体sockaddr，其定义如下：
```c
#include <bits/socket.h>
struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
};
typedef unsigned short int sa_family_t;
```

sa_family成员是地址族类型(sa_family_t)的变量。地址族类型通常与协议族类型对应。常见的协议族(protocol family，也称domain)和对应的地址族如下所示：

|协议族|地址族|描述|
|---|---|---|
|PF_UNIX|AF_UNIX|UNIX本地域协议族|
|PF_INET|AF_INET|TCP/IPv4协议族|
|PF_INET6|AF_INET6|TCP/IPv6协议族|

宏PF_\*和AF_\*都定义在bits/socket.h头文件中，且两者有完全相同的值，所以两者经常混用。

sa_data成员用于存放socket地址值。但是不同的协议族的地址值具有不同的含义和长度，如下所示：

|协议族|地址值含义和长度|
|---|---|
|PF_UNIX|文件的路径名，长度可达到108字节|
|PF_INET|16bit端口号和32bit IPv4地址，共6字节|
|PF_INET6|16bit端口号，32bit流标识，128bit IPv6地址，32bit范围ID，共26字节|

由上表可知，14字节的sa_data根本无法容纳多数协议族的地址值。因此，Linux定义了下面这个新的通用的socket地址结构体，这个结构体不仅提供了足够大的空间用于存放地址值，而且是内存对齐的。

```c
#include <bits/socket.h>
struct sockaddr_storage {
    sa_family_t sa_family;
    unsigned long int __ss_align;
    char __ss_padding[ 128 - sizeof(__ss_align) ];
};
typedef unsigned short int sa_family_t;
```

## 专用socket地址

很多网络编程函数诞生早于IPv4协议，那时候都使用的是 struct sockaddr结构体，为了向前兼容，现在sockaddr退化成了（void *）的作用，传递一个地址给函数，至于这个函数是sockaddr_in还是sockaddr_in6，由地址族确定，然后函数内部再强制类型转化为所需的地址类型。

![](zzimages/20230524143051.png)

UNIX本地域协议族使用如下专用的socket地址结构体：
```c
#include <sys/un.h>
struct sockaddr_un {
    sa_family_t sin_family;
    char sun_path[108];
};
```

TCP/IP协议族有sockaddr_in和sockaddr_in6两个专用的socket地址结构体，它们分别用于IPv4和IPv6：
```c
#include <netinet/in.h>
struct sockaddr_in {
    sa_family_t sin_family; /* __SOCKADDR_COMMON(sin_) */
    in_port_t sin_port; /* Port number. */
    struct in_addr sin_addr; /* Internet address. */
    /* Pad to size of `struct sockaddr'. */
    unsigned char sin_zero[sizeof (struct sockaddr) - __SOCKADDR_COMMON_SIZE -
                sizeof (in_port_t) - sizeof (struct in_addr)];
};

struct in_addr {
    in_addr_t s_addr;
};

struct sockaddr_in6 {
    sa_family_t sin6_family;
    in_port_t sin6_port; /* Transport layer port # */
    uint32_t sin6_flowinfo; /* IPv6 flow information */
    struct in6_addr sin6_addr; /* IPv6 address */
    uint32_t sin6_scope_id; /* IPv6 scope-id */
};

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
#define __SOCKADDR_COMMON_SIZE (sizeof (unsigned short int))
```

所有专用socket地址（以及sockaddr_storage）类型的变量在实际使用时都需要转化为通用socket地址类型sockaddr（强制转化即可），因为所有socket编程接口使用的地址参数类型都是sockaddr。

# 4. IP地址转换

通常，人们习惯用可读性好的字符串来表示 IP 地址，比如用点分十进制字符串表示 IPv4 地址，以及用十六进制字符串表示 IPv6 地址。但编程中我们需要先把它们转化为整数（二进制数）方能使用。而记录日志时则相反，我们要把整数表示的 IP 地址转化为可读的字符串。下面 3 个函数可用于用点分十进制字符串表示的 IPv4 地址和用网络字节序整数表示的 IPv4 地址之间的转换：
```c
#include <arpa/inet.h>
in_addr_t inet_addr(const char *cp);
int inet_aton(const char *cp, struct in_addr *inp);
char *inet_ntoa(struct in_addr in);
```

下面这对更新的函数也能完成前面 3 个函数同样的功能，即将IP地址字符串与网络字节序的整数进行转换，并且它们同时适用 IPv4 地址和 IPv6 地址：
```c
#include <arpa/inet.h>
// p表示点分十进制的IP字符串，n表示network，即网络字节序的整数
int inet_pton(int af, const char *src, void *dst);
/*
    - af表示地址族，可选AF_INET或AF_INET6
    - src表示需要转换的点分十进制的IP字符串
    - dst用于存放转换后的结果
*/

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
/*
    - af表示地址族，可选AF_INET或AF_INET6
    - src是要转换的网络字节序的整数的地址
    - dst用于存放转换成IP地址的字符串
    - size指明第三个参数的大小（数组的大小）
    - 返回值：返回转换后的字符串的地址，和dst相同
*/
```

# 5. TCP通信流程

服务器端：

1. 创建一个用于监听的套接字，来监听客户端的连接。这个套接字其实就是一个文件描述符。
2. 将这个监听文件描述符和本地的IP和端口绑定（IP和端口就是服务器的地址信息）。客户端连接服务器的时候使用的就是这个IP和端口。
3. 设置监听，监听套接字开始工作。
4. 阻塞等待，当有客户端发起连接，解除阻塞，接受客户端的连接，然后得到一个和客户端通信的新的套接字。
5. 通信(接收数据和发送数据)。
6. 通信结束，断开连接。

客户端：

1. 创建一个用于通信的套接字
2. 连接服务器，需要指定连接的服务器的IP和端口
3. 连接成功后，客户端可以直接和服务器通信(接收数据和发送数据)
4. 通信结束，断开连接

![](zzimages/20230524163943.png)

# 6. socket函数

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> //包含了这个头文件，上面两个就可以省略
```

```c
int socket(int domain, int type, int protocol);
```
- 功能：创建一个套接字
- 参数：
  - domain: 协议族。AF_INET表示ipv4；AF_INET6表示ipv6；AF_UNIX或AF_LOCAL表示本地套接字通信（即本地进程间通信）
  - type: 通信过程中使用的协议类型。SOCK_STREAM表示流式协议，SOCK_DGRAM表示报式协议
  - protocol: 具体的一个协议，一般传入0表示默认。type为SOCK_STREAM时默认协议是TCP，SOCK_DGRAM时默认协议是UDP
- 返回值：成功则返回文件描述符，操作的就是内核缓冲区；失败返回-1

```c
int bind(int sockfd, const struct sockaddr *addr,
                socklen_t addrlen);
```
- 功能：绑定，将fd和本地的IP+端口进行绑定，也称为socket命名
- 参数：
  - sockfd : 通过socket()函数得到的文件描述符
  - addr : 需要绑定的socket地址，这个地址封装了ip和端口号的信息
  - addrlen : 第二个参数结构体占的内存大小
- 返回值：成功返回0，失败返回-1

```c
int listen(int sockfd, int backlog);
```
- 功能：监听这个socket上的连接
- 参数：
  - sockfd : 通过socket()函数得到的文件描述符
  - backlog : 未连接的和已经连接的和的最大值，一般传入8即可，最大上限可在/proc/sys/net/core/somaxconn文件中查看
- 返回值：成功返回0，失败返回-1

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- 功能：接收客户端连接，默认是一个阻塞的函数，阻塞等待客户端连接
- 参数：
  - sockfd : 用于监听的文件描述符
  - addr : 传出参数，记录了连接成功后客户端的地址信息（ip，port）
  - addrlen : 指定第二个参数的对应的内存大小
- 返回值：成功则返回用于通信的文件描述符，失败返回-1

```c
int connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen);
```
- 功能：客户端连接服务器
- 参数：
  - sockfd : 用于通信的文件描述符
  - addr : 客户端要连接的服务器的地址信息
  - addrlen : 第二个参数的内存大小
- 返回值：成功返回0，失败返回-1

```c
ssize_t write(int fd, const void *buf, size_t count); //写数据
ssize_t read(int fd, void *buf, size_t count); //读数据
```

