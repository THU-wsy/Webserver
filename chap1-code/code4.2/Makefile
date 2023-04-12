#定义变量
#src=add.c mult.c test.c
#obj=add.o mult.o test.o
src=$(wildcard *.c)
obj=$(patsubst %.c, %.o, $(src))
tar=test

$(tar):$(obj)
	$(CC) $^ -o $@

$.o:%.c
	$(CC) -c $< -o $@

#定义伪目标，不会生成特定的clean文件。下面的clean用于清除.o文件，在命令行输入make clean即可。
.PHONY:clean
clean:
	rm $(obj) -f