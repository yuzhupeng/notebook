# Wireshark 基础篇

<aside>
💡 注: 有些指令可能无法使用,但是可以去查,,,我们只需要知道他有那么一个功能就可以了


捕获过滤器: 在接收的时候进行一个过滤

显示过滤器: 捕获所有,对结果做一个筛选





参考链接

[wireshark网络安全流量分析基础_小V讲网安的博客-CSDN博客](https://blog.csdn.net/xv66666/article/details/131535667)

[WireShark 过滤http请求_wireshark过滤http_zzy0609的博客-CSDN博客](https://blog.csdn.net/zzy0609/article/details/95455136)

[APT攻击溯源项目学习二：流量分析](https://www.jianshu.com/p/5930bb2dcf00)

过滤器应用

掌握过滤器的应用对于数据分析来说是一项必备技能，可以说是一项大海捞针的技巧。学会构建，编辑，保存关键的显示过滤器能够大量的节省时间。

协议过滤器

- arp：显示所有包括ARP请求和回复在内的所有ARP数据流。
- ip：显示内含IPv4头在内的（如ICMP目的地址不可达报文，在ICMP报文头之后返回到来方向的IPv4头）IP数据流。
- ipv6：显示所有IPv6数据流，包括内含IPv6报文头的IPv4报文，如6to4，Teredo，以及ISATAP数据流。
- tcp：显示所有基于TCP的数据流。

应用过滤器

- bootp：显示所有DHCP数据流（基于BOOTP）。
- dns：显示包括TCP区域传输以及基于标准UDP的DNS请求和回复在内的所有DNS数据流。
- tftp：显示所有TFTP（Trivial File Transfer Protocol）数据流。
- http：显示所有HTTP命令，回复以及数据传输报文，但不显示TCP握手报文，TCP ACK报文以及TCP结束报文。
- icmp：显示所有ICMP报文

# ip过滤

```c
ip.src eq xxx.xxx.xxx.xxx 
ip.dst==xxx.xxx.xxx.xxx 
ip.addr eq xxx.xxx.xxx.xxx
```

# 端口过滤

```c
tcp.port == 80 or udp.port == 80  
tcp.dstport == 80 //只显示tcp协议的目标端口为80
tcp.srcport == 80 //只显示tcp协议的源端口为80
tcp.port >=1 and tcp.port <=80
```

# MAC地址过滤

```c
eth.src == xx:xx:xx:xx:xx:xx
eth.dst == xx:xx:xx:xx:xx:xx
eth.addr == xx:xx:xx:xx:xx:xx
```

# 协议过滤

```c
tcp||udp||arp||icmp||http||ftp||dns||ip

tcp，只显示TCP协议的数据包列表

http，只查看HTTP协议的数据包列表

icmp，只显示ICMP协议的数据包列表
...

```

# HTTP模式过滤

```c
HTTP

http.server //过滤所有含有http头中含有server字段的数据包
HTTP.request.method == "GET" //过滤所有请求方式为POST的http请求包，注意POST为大写
HTTP.request.method =="POST"
HTTP.request.uri == "/img/logo-edu.gif"
HTTP contains "WVS"————比较常用
http.cookie contains guid //过滤含有指定cookie的http数据包
http.request.uri=="/online/setpoint" //过滤请求的uri，取值是域名后的部分
http.request.full_uri== "http://task.browser.360.cn/online/setpoint "//过滤含域名的整个url则需要使用http.request.full_uri
http.server contains "nginx" //过滤http头中server字段含有nginx字符的数据包
http.content_type == "text/html" //过滤content_type是text/html的http响应、post包，即根据文件类型过滤http数据包
http.content_encoding == "gzip" //过滤content_encoding是gzip的http包
http.transfer_encoding == "chunked" //根据transfer_encoding过滤
http.content_length == 279 //根据content_length的数值过滤
http.content_length_header == "279"
http.request.version == "HTTP/1.1" //过滤HTTP/1.1版本的http包，包括请求和响应
http.response.phrase == "OK" //过滤http响应中的phrase
```

过滤经过指定域名的http数据包，这里的host值不一定是请求中的域名

```c
http.host==magentonotes.com
http.host contains magentonotes.com
```

//过滤http响应状态码为302的数据包

```c
http.response.code==302
```

http.response==1

//过滤所有的http响应包

http.request==1

//过滤所有的http请求，貌似也可以使用http.request

# 追踪流 (未实践)

如果要看某一个报文中的数据，可以右键目标报文，选择追踪流，再选择不同的流，

可以看到TCP流，UDP流，SSL流和HTTP流。这样就可以看到服务端和客户端之间传输的数据内容

![Untitled](Wireshark%20%E5%9F%BA%E7%A1%80%E7%AF%87%20f07aafe54259462da43366e3c08232e9/Untitled.png)