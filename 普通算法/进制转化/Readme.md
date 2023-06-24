---
layout: wiki
wiki: 汇编小项目
title: win32汇编实现任意进制转化
---



# 参考链接

https://blog.csdn.net/huangxy10/article/details/8310087

https://www.cnblogs.com/cloudeman/archive/2012/04/09/2439438.html



其实也就是我们在汇编语言中,可以调用很多C语言的函数

比如下面涉及的函数

```
stdio.h
stdlib.h
string.h
```



常用的CRT函数

```
crt_memcpy
crt_memset
crt_strcpy
crt_system
crt_printf
crt_scanf
crt_fopen
crt_gets
...
```



其实涉及的很多的函数例子你就都可以加上`crt_`前缀,然后尝试一下可不可以正常使用该函数

能用,当然就万事大吉啦

实践
