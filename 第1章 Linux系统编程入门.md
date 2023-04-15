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

动态库的制作：首先通过gcc获得.o文件，==得到和位置无关的代码==
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

- 静态库的优点：静态库被打包到应用程序中加载速度快、发布程序无需提供静态库从而移植方便。
- 静态库的缺点：消耗系统资源浪费内存(多个应用程序包含同一个静态库，该静态库会被加载多份至内存)、更新部署发布麻烦。
- 动态库的优点：可以实现进程间资源共享(共享库)、更新部署发布简单(只需更新动态库即可，应用程序无需重新编译)、可以控制何时加载动态库。
- 动态库的缺点：加载速度比静态库慢、发布程序时需要提供依赖的动态库。

# 4. Makefile

一个Makefile文件中可以有一个或者多个规则
```
目标 ... : 依赖 ...
    命令(Shell命令)
    ...
```
- 目标：最终要生成的文件(伪目标除外)
- 依赖：生成目标所需要的文件或者目标
- 命令：通过执行命令对依赖操作生成目标(命令前必须Tab缩进)

Makefile中的其他规则一般都是为第一条规则服务的。(一般来说，如果其他规则与第一条规则无关，则不会执行其他规则，只会执行第一条规则)

工作原理：
- 命令在执行之前，需要先检查规则中的依赖是否存在(如果存在，则执行命令；否则，向下检查其他规则，检查有没有一个规则是用来生成这个依赖的，如果找到了就执行该规则中的命令)
- 检测更新，在执行规则中的命令时，会比较目标和依赖文件的时间(如果依赖的时间比目标的时间晚，需要重新生成目标；否则，目标不需要更新，对应规则中的命令不需要被执行)

## 变量

自定义变量：变量名=变量值

预定义变量：
- AR：归档维护程序的名称，默认值为ar
- CC：C编译器的名称，默认值为cc
- CXX：C++编译器的名称，默认值为g++
- \$@：目标的完整名称
- \$<：第一个依赖文件的名称
- \$^：所有的依赖文件

获取变量的值：\$(变量名)

示例：
```makefile
test:add.c mult.c test.c
    gcc add.c mult.c test.c -o test
```
可以用以下语句替换
```makefile
test:add.c mult.c test.c
    $(CC) $^ -o $@
```

## 模式匹配

\%.o:\%.c
其中\%是通配符，匹配一个字符串，注意两个\%匹配的是同一个字符串。

示例：
```makefile
add.o:add.c
	gcc -c add.c -o add.o
mult.o:mult.c
	gcc -c mult.c -o mult.o
```
可以简写为
```makefile
%.o:%.c
    gcc -c $< -o $@
```

## 函数

\$(wildcard PATTERN...)
- 功能：获取指定目录下指定类型的文件列表
- 参数：PATTERN指的是某个或多个目录下的对应的某种类型的文件，如果有多个目录，一般使用空格间隔
- 返回：得到若干个文件的文件列表，文件名之间使用空格间隔

示例：
```makefile
$(wildcard *.c ./sub/*.c)
```
表示获取当前目录下所有.c文件以及当前目录下的sub子目录下的所有.c文件。返回值格式：a.c b.c c.c d.c e.c f.c

\$(patsubst \<pattern\>,\<replacement\>,\<text\>)
- 功能：查找\<text\>中的单词(单词以空格、Tab、回车或换行分隔)是否符合模式\<pattern\>，如果匹配的话，则以\<replacement\>替换。
- \<pattern\>可以包括通配符\%，表示任意长度的字串。如果\<replacement\>中也包含\%，那么\<replacement\>中的这个\%将是\<pattern\>中的那个\%所代表的的字串
- 返回：函数返回被替换过后的字符串

示例：
```makefile
$(patsubst %.c, %.o, x.c bar.c)
```
返回值格式：x.o bar.o

# 5. GDB

## 准备工作

通常，在为调试而编译时，我们会关掉编译器的优化选项-O，并打开调试选项-g，另外，-Wall在尽量不影响程序行为的情况下选项打开所有warning，也可以发现许多问题，避免一些不必要的BUG。
示例：
```shell
gcc -g -Wall test.c -o test
```
-g选项的作用是在可执行文件中加入源代码的信息，比如可执行文件中第几条机器指令对应源代码的第几行，但并不是把整个源文件嵌入到可执行文件中，所以在调试时必须保证gdb能找到源文件。

## GDB命令

启动和退出：
- gdb 可执行程序
- q(或者quit)

给程序设置参数/获取设置参数：
- set args 10 20
- show args

GDB使用帮助：
- help

查看当前文件代码
- list/l (从默认位置显示)
- list/l 行号 (从指定的行显示)
- list/l 函数名 (从指定的函数显示)

查看非当前文件代码
- list/l 文件名:行号
- list/l 文件名:函数名

设置显示的行数
- show list/listsize
- set list/listsize 行数

设置断点
- b/break 行号
- b/break 函数名
- b/break 文件名:行号
- b/break 文件名:函数

查看断点
- i/info b/break

删除断点
- d/del/delete 断点编号

设置断点无效
- dis/disable 断点编号

设置断点生效
- ena/enable 断点编号

设置条件断点(一般用在循环的位置)
- b/break 10 if i\=\=5

运行GDB程序
- start (程序停在第一行)
- run (遇到断点才停)

继续运行，到下一个断点停
- c/continue

向下执行一行代码(不会进入函数体)
- n/next

变量操作
- p/print 变量名(打印变量值)
- ptype 变量名(打印变量类型)

向下单步调试(遇到函数进入函数体)
- s/step
- finish (跳出函数体)

自动变量操作
- display num (自动打印指定变量的值)
- i/info display
- undisplay 编号

其他操作
- set var 变量名=变量值
- until (跳出循环)






