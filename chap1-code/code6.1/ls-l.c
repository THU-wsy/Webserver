//模拟实现ls -l 指令
//-rw-r--r-- 1 wsy wsy 36 Apr 16 18:57 file.txt

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h> //包含函数getpwuid
#include <grp.h> //包含函数getgrgid
#include <time.h>
#include <string.h>


int main(int argc, char* argv[]) {

    //判断输入的参数是否正确
    if (argc < 2) {
        printf("%s filename\n", argv[0]);
        return -1;
    }

    //通过stat函数获取用户传入的文件的信息
    struct stat st;
    int ret = stat(argv[1], &st);
    if (ret == -1) {
        perror("stat");
        return -1;
    }

    //获取文件类型和文件权限
    char perms[11] = {'\0'};
    switch (st.st_mode & __S_IFMT) {
        case __S_IFLNK:
            perms[0] = 'l';
            break;
        case __S_IFDIR:
            perms[0] = 'd';
            break;
        case __S_IFBLK:
            perms[0] = 'b';
            break;
        case __S_IFCHR:
            perms[0] = 'c';
            break;
        case __S_IFSOCK:
            perms[0] = 's';
            break;
        case __S_IFREG:
            perms[0] = '-';
            break;
        case __S_IFIFO:
            perms[0] = 'p';
            break;
        default:
            perms[0] = '?';
            break;
    }

    perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';

    //获取硬链接数
    int linkNum = st.st_nlink;

    //文件所有者
    char* fileUser = getpwuid(st.st_uid)->pw_name;

    //文件所在组
    char* fileGrp = getgrgid(st.st_gid)->gr_name;

    //文件大小
    long int fileSize = st.st_size;

    //获取修改的时间
    char* time = ctime(&st.st_mtime);
    char mtime[512] = {0};
    strncpy(mtime, time, strlen(time) - 1);

    char buf[1024];
    sprintf(buf, "%s %d %s %s %ld %s %s", perms, linkNum, fileUser, fileGrp, fileSize, mtime, argv[1]);
    printf("%s\n", buf);

    return 0;
}