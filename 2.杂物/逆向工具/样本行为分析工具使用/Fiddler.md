# Fiddler

Fiddler的功能类似于Burp suit

详细的介绍

[Fiddler抓包：详解Fiddler抓包工具软件使用教程_fiddler抓包工具使用教程_陪代码一起浪迹天涯的博客-CSDN博客](https://blog.csdn.net/seoyundu/article/details/121067090)

# 基础配置

![Untitled](Fiddler%2035c179059c1c4e7b8fba982c5a3a7547/Untitled.png)

1),

![Untitled](Fiddler%2035c179059c1c4e7b8fba982c5a3a7547/Untitled%201.png)

选上那个,一开始这个页面其实只有2个选项,就下面这2

![Untitled](Fiddler%2035c179059c1c4e7b8fba982c5a3a7547/Untitled%202.png)

我们要勾住红色的,才会有很多选项的出现

期间涉及管理员权限的操作

然后再象征性的点击一下下面这个,,,

![Untitled](Fiddler%2035c179059c1c4e7b8fba982c5a3a7547/Untitled%203.png)

2), 关于端口

![Untitled](Fiddler%2035c179059c1c4e7b8fba982c5a3a7547/Untitled%204.png)

注意这个端口要和Proxifier设置的代理端口一致

 

![Untitled](Fiddler%2035c179059c1c4e7b8fba982c5a3a7547/Untitled%205.png)

关于怎么开始监听

点击Capturing,它只会监听来自8888端口的流量

也就是Proxifier配置那个

如果不点击它,他会监听所有进程的所有流量

点击后,他会变为透明,看不见

![Untitled](Fiddler%2035c179059c1c4e7b8fba982c5a3a7547/Untitled%206.png)

左边是监听的报文,右边是具体的报文内容

跟多的用法可以以后再写,或者参考顶部的链接

比如如何过滤,如何拦截等等