# 1. GCC

编译生成可执行文件：

```shell
gcc test.c -o test
```

运行可执行文件：
```shell
./test
```

![](zzimages/20230411191701.png)

![](zzimages/20230411191829.png)

![](zzimages/20230411191908.png)


```shell
gcc test.c -E -o test.i
gcc test.i -S -o test.s
gcc test.s -c -o test.o
```

# 2. 静态库

命名规则：libxxx.a

静态库的制作：首先通过gcc获得.o文件，再使用ar工具将它们打包。
```shell
ar rcs libxxx.a xxx.o xxx.o
```

静态库的使用：
```shell
gcc test.c -o test -l xxx
```

# 3. 动态库

命名规则：libxxx.so

动态库的制作：首先通过gcc获得.o文件，得到和位置无关的代码
```shell
gcc -c -fpic a.c b.c
```
再通过gcc得到动态库
```shell
gcc -shared a.o b.o -o libcalc.so
```

动态库的使用：
```shell
gcc test.c -o test -l calc
```

工作原理：对于静态库，gcc进行链接时会把静态库中代码打包到可执行程序中；而对于动态库，gcc进行链接时不会把动态库的代码打包到可执行程序中，而是在程序启动之后，动态库才会被动态加载到内存中，通过ldd(list dynamic dependencies)命令检查动态库依赖关系。

定位共享库文件的方法：当系统加载可执行代码时候，能够知道其所依赖的库的名字，但是还需要知道绝对路径，此时就需要系统的动态载入器来获取该绝对路径。对于elf格式的可执行程序，是由ld-linux.so来完成的，它先后搜索elf文件的DT_RPATH段、环境变量LD_LIBRARY_PATH、/etc/ld.so.cahce文件列表、/lib和/usr/lib目录找到库文件后将其载入内存。

方法一：
```shell
vim /home/wsy/.bashrc
```
在最后一行添加动态库的绝对路径
```shell
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/wsy/WebServer/chap1-code/code3/lib
```
然后输入命令刷新即可
```shell
source /home/wsy/.bashrc
```

方法二：
```shell
sudo vim /etc/ld.so.conf
```
在第二行添加动态库的绝对路径
```shell
/home/wsy/WebServer/chap1-code/code3/lib
```
保存退出后更新即可
```shell
sudo ldconfig
```

方法三：直接将动态库放到/lib或/usr/lib目录下即可。但这种方法并不推荐。