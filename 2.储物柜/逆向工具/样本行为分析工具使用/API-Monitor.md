# API-Monitor

64位版本只能用来监视64位应用程序。 

32位版本仅可用于监测的32位应用程序。 

目前,没有看到什么教程之类的.只有一个官方的翻译文档,也不叫教程

[API Monitor简介（API监控工具）-阿里云开发者社区](https://developer.aliyun.com/article/644566)

官网的文档(不叫教程)

[API Monitor: Spy on API Calls and COM Interfaces (Freeware 32-bit and 64-bit Versions!) | rohitab.com](http://www.rohitab.com/apimonitor?spm=a2c6h.12873639.article-detail.31.53a64a0aYrTa4b)

我是怎么了解到这些用法的?

答: 问Chatgpt

# 初步使用

如何开启检测?

这边的全部,最好全部勾上

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled.png)

如果是启动一个新的进程并检测的话

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%201.png)

然后就是一路简单的选择一下

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%202.png)

点击ok就完成启动

结果如下

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%203.png)

很人性化的是,把鼠标对到某个API.就会显示参数

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%204.png)

下方是一些堆栈,函数调用等等信息

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%205.png)

# 只显示特定模块

比如我们的exe是C.exe

在显示的时候,有很多模块的加载,比如什么kernel32,什么user32等等

我们只想显示C.exe

选择中间那个

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%206.png)

添加一个规则

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%207.png)

选择当前正在调用的模块名字

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%208.png)

输入C.exe

就会只显示C,exe

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%209.png)

如果要过滤特定的dll,也是选择 calling Module Name

# 不足

API调用也还好

如果调用得过于底层,API monitor就无能为力

比如x64程序通过syscall调用API

那么,API monitot 就g了

# bug

![Untitled](API-Monitor%206cd93e200fa143419580d6190c65b93c/Untitled%2010.png)

查资料告诉我

当我**dwCreationFlags=0 不是** CREATE_SUSPENDED,而是值得其它选项

所以的话,就很离谱的