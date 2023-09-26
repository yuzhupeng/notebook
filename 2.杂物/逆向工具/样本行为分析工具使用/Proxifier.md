# Proxifier

一款代理软件

可以把指定进程的流量传递给某个端口

具体配置如下

# 前提

先开启一个设置

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled.png)

然后勾选

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled%201.png)

然后才是具体的配置

# 配置监听的端口

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled%202.png)

配置ip

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled%203.png)

比如burp用的是8080端口监听

我们就添加一个8080的代理

密码和用户名可以设置一个,通过后面发现,,,设置一个..报错就少些

在Protocol,我们选择的是HTTPS.其实还可以选择其他的

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled%204.png)

那个端口我们得注意一下

那是监听器的端口

Proxifier只是一个代理软件

监听器的端口有burp suit 或者 Fiddler

burp suit的监听端口是8080

Fiddler的监听端口是8888

代理抓取到的流量都会送往监听器

重要的事情说3遍:

Proxifier配置后,pc的所有流量都会走Proxifier,

Proxifier会分发流量

1), 阻塞它

2), 让它溜走

3), 送给监听器,自己去处理, 比如给burp suit 或者Fiddler

# 配置监听进程

其实就是要监听哪一个进程的流量

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled%205.png)

比如我们这里

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled%206.png)

我们只监听QQ.exe进程的任何数据

对于默认的应用,我们选择Direct,直连, 不监听

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled%207.png)

监听的行为有几个

![Untitled](Proxifier%20ddb6b151cd1b401aaac06a39ab90a6ca/Untitled%208.png)

阻塞,

直连,

走代理,