

```
https://blog.csdn.net/qq_28351609/article/details/114855630
https://blog.csdn.net/Luckiers/article/details/124568399
```



# 基本的调试操作



进入gdb

```
gdb -q xx.elf
```

`gdb`启动时会默认打印一堆免责条款，通过添加 `--silent`（或者 `-q`、`--quiet`）选项，可将这部分信息屏蔽掉。



然后设置断点

```
b xxx位置
```



然后开始运行

```
run 类似于F9
continue(c) 类似于F9
```



停在断点以后,开始调试

```
step(s) 调试源代码的f7
next(n) 调试源代码的f8
si 调试汇编的f7
ni 调试汇编的f8

i b 打印所有断点
delet 在打印之后,删除对应的断点

```

其它

```
start: 执行程序至main()主函数的起始位置,可是不是有调试符号的文件的主函数名字叫做main
run(r) 一直执行程序，直到执行结束。如果程序中手动设置有断点，则 run指令会执行程序至第一个断点处
jmp(j) 类似于F4
clear 命令可以删除指定位置处的所有断点, (gdb): clear location
```