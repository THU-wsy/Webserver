首先用gcc将src中的源文件编译成.o文件
```shell
gcc -c add.c div.c mult.c sub.c -I ../include
```

再使用ar工具将它们打包
```shell
ar rcs libcalc.a add.o div.o mult.o sub.o
```

然后将libcalc.a移动至lib目录下
```shell
mv libcalc.a ../lib
rm -rf *.o
```

最后使用该静态库
```shell
gcc test.c -o test -I ./include -L ./lib -l calc
```
