#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {

    // 1. 通过open打开file
    int srcfd = open("file.txt", O_RDONLY);
    if (srcfd == -1) {
        perror("warn");
        return -1;
    }

    // 2. 创建一个新的文件(拷贝文件)
    int destfd = open("cpy.txt", O_WRONLY | O_CREAT, 0664);
    if (destfd == -1) {
        perror("warn");
        return -1;
    }

    // 3. 频繁的读写操作
    char buf[1024] = {0};
    int len = 0;
    while ((len = read(srcfd, buf, sizeof(buf))) > 0)
        write(destfd, buf, len);
    
    // 4. 关闭文件
    close(srcfd);
    close(destfd);

    return 0;
}