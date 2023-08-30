# AppInit_Dlls(不太行)



# 威力

使用注册表去注入

windwos操作系统的注册表中默认提供了AppInit_Dlls和LoadAppInit_Dlls这两个注册表项

在注册表编辑器中,把dll的路径写入AppInit_Dlls的项目中,然后把LoadAppInits_Dlls的项目设置为1

重启后,指定dll就会注入到所有运行进程(说法过于绝对)

书上说他的原理:

user32.dll被加载到进程的时候,会读取App_Init_dlls注册表项, 如果该值不为空,这调用LoadLibiary去加载用户那个的Dll

所以,严格的来说,相应的dll不会加载到所有的进程

而只是加载到user32.dll所在的进程

对于winXP而言,会忽略LoadAppinit_Dlls的注册表项

对应的路径

```c
计算机\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows
```

![Untitled](AppInit_Dlls(%E4%B8%8D%E5%A4%AA%E8%A1%8C)%20c4a2bba7bdd845ca8388532c988efb8b/Untitled.png)

AppInit_Dlls注册表 功能很强大,通过它可以向任何进程注入dll文件

如果注入的dll有问题,那么可能导致所有的windwos进程无法正常启动

![Untitled](AppInit_Dlls(%E4%B8%8D%E5%A4%AA%E8%A1%8C)%20c4a2bba7bdd845ca8388532c988efb8b/Untitled%201.png)

但是，在win7上屡试不爽，win10系统之后发现，注入方法失效

# 后来的状况

说起来很厉害

但是..微软从win8就开始限制这个功能

Windows 8 开始 就采用 UEFI 和 secure boot来提高整体系统完整性并提供针对复杂威胁的强大保护。

启用secure boot 后，AppInit_DLLs 机制将被禁用，作为保护客户免受恶意软件和威胁的不妥协方法的一部分。

secure boot 是 UEFI 协议，而不是 Windows 8 功能。

同时:Windows 8 桌面应用程序的认证要求之一是应用程序不得加载任意 DLL 以拦截使用 AppInit_DLLs 机制的 Win32 API 调用。

但是…我用vmware去关闭secure boot ,找不到那个选项

找了很久,,,,唉,,,不找了,,,没意思,,,可行性不强

# 有待参考的链接

[https://www.ctfiot.com/29349.html](https://www.ctfiot.com/29349.html)