首先用gcc将src中的源文件编译成.o文件，到和位置无关的代码-fpic
```shell
gcc -c -fpic add.c div.c mult.c sub.c -I ../include
```

再通过gcc得到动态库
```shell
gcc -shared *.o -o libcalc.so
```

然后将libcalc.so移动至lib目录下
```shell
mv libcalc.so ../lib
rm -rf *.o
```

最后使用该动态库
```shell
gcc test.c -o test -I ./include -L ./lib -l calc
```
注意要配置动态库的绝对路径才能运行test程序。