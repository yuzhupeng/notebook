# Linux配置问题

# IDA动态调试问题

关于动态调试,就不多说了

动态调试报错1

```
┌──(kali㉿kali)-[~/code]
└─$ ./hardCpp
./hardCpp: error while loading shared libraries: libc++.so.1: cannot open shared object file: No such file or directory
```

解集办法

```
apt install libc++-dev
```

[https://blog.csdn.net/qq_21920075/article/details/126750473](https://blog.csdn.net/qq_21920075/article/details/126750473)

# 关于用户创建问题

创建后没家目录,奇怪

root创建的用户 普通用户无法su

所以就用普通用户创建用户把

```bash
sudo useradd -m username
```

创建家目录

```bash
sudo mkhomedir_helper username
```

# sh切换

当前使用的sh

```bash
echo $SHELL
```

有哪些sh

```bash
cat /etc/shells
```

切换sh

```bash
chsh -s /bin/bash
```

# 端口开放

要在Ubuntu上打开端口8080，您可以按照以下步骤操作：

1. 首先，确保您已以root或具有管理员权限的用户身份登录到Ubuntu系统。
2. 打开终端（Terminal）应用程序。
3. 使用以下命令编辑防火墙配置文件：`sudo vim /etc/ufw/ufw.conf`
4. 找到**`ENABLED`**行，并将其更改为： `ENABLED=yes`
    
    这将启用Ubuntu的防火墙（Uncomplicated Firewall）。
    
5. 保存更改并关闭编辑器（如果您使用的是Nano编辑器，则按下**`Ctrl+X`**，然后按下**`Y`**以保存更改）。
6. 然后，使用以下命令打开8080端口： `sudo ufw allow 8080`
    
    这将在防火墙中添加一条规则，允许传入的连接通过8080端口。
    
7. 最后，启用防火墙以应用更改： `sudo ufw enable`
    
    系统将询问您是否要继续。输入**`y`**并按下Enter键。
    

现在，您应该已经成功打开了Ubuntu上的8080端口。请注意，如果您正在运行其他安全软件或服务，它们可能会干扰端口的访问。确保您的网络配置和其他防火墙规则不会阻止对8080端口的访问。