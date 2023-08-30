# winDbg

# workspace

1. View->Option->Wrokspace Prompts 改为Always Ask
2. 删除WorkSpace，所有的都删除
3. 修改成你想要的字体
4. 关闭Windbg，会弹出一个对话框，勾选，点击YES.

# 查看

| 指令 | 功能 |
| --- | --- |
| r | 查看寄存器 |
| dd | 以四字节为一组读取并显示数据 |
| dq | 以八字节为一组读取并显示数据 |

dd 4个字节的查看

```
 0:000> dd 0x00400000
 00400000  00905a4d 00000003 00000004 0000ffff
 00400010  000000b8 00000000 00000040 00000000
 00400020  00000000 00000000 00000000 00000000
 00400030  00000000 00000000 00000000 000000f0
 
```

dq 8个字节的查看

```
 0:000> dq 0x00400000
 00400000  00000003`00905a4d 0000ffff`00000004
 00400010  00000000`000000b8 00000000`00000040
 00400020  00000000`00000000 00000000`00000000
 
```

dd 0x00400000 L40

意思是显示40个dword出来

# 调试

## 双机调试

当程序一致时busy的时候,

看向工具栏,然后选择debug

然后会出现一个ctrl break的选项

选他就可以停住虚拟机

## xp

> xp配置
> 

C盘根目录隐藏文件:boot.ini

```
 [boot loader]
 timeout=30
 default=multi(0)disk(0)rdisk(0)partition(1)\WINDOWS
 [operating systems]
 multi(0)disk(0)rdisk(0)partition(1)\WINDOWS="Microsoft Windows XP Professional" /noexecute=optin /fastdetect
 multi(0)disk(0)rdisk(0)partition(1)\WINDOWS="XP[debug]" /debug /debugport=com1 /noexecute=optin /fastdetect
```

> 虚拟机配置:
> 

使用命名管道

```
 \\.\pipe\com_1
```

轮询时主动放弃CPU

该端时服务器

另外一端时应用程序

> windbgx86配置
> 

```
"D:\Windows Kits\10\Debuggers\x86\windbg.exe" -y srv*c:\symbols*http://msdl.microsoft.com/download/symbols -b -k com:port=\\.\pipe\com_1,baud=115200,pipe
```

# 资源链接

[一个比较全|乱的使用说明](https://www.cnblogs.com/csnd/p/11515280.html)