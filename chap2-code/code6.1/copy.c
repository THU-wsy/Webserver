//使用内存映射实现文件拷贝的功能
/*
    1. 对原始的文件进行内存映射
    2. 创建一个新文件(拓展该文件)
    3. 把新文件的数据映射到内存中
    4. 通过内存拷贝将原始文件的内存数据拷贝到新文件的内存中
    5. 释放资源
*/

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    int fd = open("origin.txt", O_RDWR);
    int len = lseek(fd, 0, SEEK_END);

    int fd1 = open("cpy.txt", O_RDWR | O_CREAT, 0664);
    truncate("cpy.txt", len);
    write(fd1, " ", 1);

    void* ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    void* ptr1 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    if (ptr == MAP_FAILED || ptr1 == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }

    memcpy(ptr1, ptr, len);
    munmap(ptr1, len);
    munmap(ptr, len);
    close(fd1);
    close(fd);

    return 0;
}
