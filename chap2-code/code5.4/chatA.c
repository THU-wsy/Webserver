#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {

    //1. 创建两个管道
    int ret = access("fifo1", F_OK);
    if (ret == -1) {
        ret = mkfifo("fifo1", 0664);
        if (ret == -1) {
            perror("mkfifo");
            exit(0);
        }
    }

    ret = access("fifo2", F_OK);
    if (ret == -1) {
        ret = mkfifo("fifo2", 0664);
        if (ret == -1) {
            perror("mkfifo");
            exit(0);
        }
    }

    //2. 只写打开fifo1
    int fdw = open("fifo1", O_WRONLY);
    if (fdw == -1) {
        perror("open");
        exit(0);
    }
    printf("打开管道fifo1成功，等待写入...\n");

    //3. 只读打开fifo2
    int fdr = open("fifo2", O_RDONLY);
    if (fdr == -1) {
        perror("open");
        exit(0);
    }
    printf("打开管道fifo2成功，等待读取...\n");

    //4. 循环地写读数据
    char buf[128];
    while (1) {
        memset(buf, 0, 128);
        fgets(buf, 128, stdin);
        ret = write(fdw, buf, strlen(buf));
        if (ret == -1) {
            perror("write");
            break;
        }

        memset(buf, 0, 128);
        ret = read(fdr, buf, 128);
        if (ret <= 0) {
            break;
        }
        printf("buf : %s\n", buf);
    }

    close(fdw);
    close(fdr);

    return 0;
}