



# vs2019/17+wdk10

环境配置:

VS2013(update4) + wdk8.1 驱动开发环境搭建 https://blog.csdn.net/hk_5788/article/details/46801647

我用的是vs2019+wdk10，安装过程属于纯小白是一路默认,一点脑袋都不动

VS2013试了很久都没有成功



项目创建和生成:

第一次创建一个项目不像C语言那么简单,函数有些东西要注意一下

可以参考链接: https://www.bilibili.com/video/BV1QJ411A7kR?p=6&vd_source=43e0e1ba96c9433bed474fdc8305c719

创建一个项目



![image-20230809201332874](img/image-20230809201332874.png)

然后删除inf文件,注意是项目里面删除,而不是移除

![image-20230809201652782](img/image-20230809201652782.png)

![image-20230809201737276](img/image-20230809201737276.png)

然后继续配置项目属性



配置属性 `->` C/C++ `->` 常规 `->` 警告等级 `->` 等级3(/W3)

配置属性 `->` C/C++ `->` 常规 `->` 将警告视为错误 `->` 否(/WX-)

配置属性 `->` C/C++ `->` 代码生成 `->` Spectre Mitigation `->` Disabled   ps: Spectre缓解库的东东

配置属性 `->` Driver Settings `->` Target OS Version `->` Windows10  ps:如果是win7就写win7

配置属性 `->` Driver Settings `->` Target Platform `->` Desktop



在空的项目的源文件添加一个 test.c 文件,如果写cpp的话,就编译不过

文件内容如下

```c
#include<ntddk.h>
void my_unload(PDRIVER_OBJECT xx)
{
    DbgPrint("Good Bye rdqx\n");
}
NTSTATUS  DriverEntry(
    PDRIVER_OBJECT driver,
    PUNICODE_STRING reg_path
)
{
    driver->DriverUnload = my_unload;//安全卸载函数
    DbgPrint("redqx: %wZ\n", reg_path);
    return 0;
}
```



然后右键生成

![image-20230809202626641](img/image-20230809202626641.png)



加载驱动的3种方式

1), 通过服务

2), ntdll的API调用 ZwLoadDriver,搭配一个注册表路径,指定手动加载还是自动加载

3), inf文件,很麻烦

4), 百度一下



但是驱动这个东东需要一个官方签名才可以加载的

但是也不是说非要签名才可以的



参考链接: https://www.bilibili.com/read/cv10382111

需要makecert.exe和signtool.exe这两个开发工具带的程序，

网上搜一下就有，capicom.dll可以到这里下载安装

生成证书

```
makecert -r -pe -ss Sparkle -n "CN=Sparkle" sparkle.cer
```

签名一下

```
signtool sign /v /s Sparkle /n Sparkle 
ps: 要签名的文件用空格隔开就 
```

就签好了 



但是签名很麻烦 

关驱动强制签名是不可能关驱动强制签名的

Windows 10 1803以后是不可能关的，但是打开测试模式可以使用自签名证书

```c
bcdedit -set NOINTEGRITYCHECKS ON
bcdedit -set TESTSIGNING ON
bcdedit -set loadoptions DDISABLE_INTEGRITY_CHECKS


//关闭那个模式 
//bcdedit /set testsigning off
```

然后重启电脑就进入了测试模式

![image-20230809215640013](img/image-20230809215640013.png)



debugview配置一下才可以监听内核的DbgPrint: 

以管理员员模式启动

![image-20230809205910249](img/image-20230809205910249.png)



然后我们就用DriverMonitor.exe 加载驱动

注意: 以管理员模式启动DriverMonitor.exe

然后

![image-20230809215850234](img/image-20230809215850234.png)

于是选择->加载->卸载

![image-20230809215953898](img/image-20230809215953898.png)

debugView就出现了消息





# vs2013+wdk8.1

先安装vs2013,然后安装wdk8.1



和之前的大体相同

但是又有一些又不相同

比如xp可以不要签名加载驱动

然后vs2013的生成修改的地方很少很少



测试文件如下

```c++
#include<ntddk.h>
void my_unload(PDRIVER_OBJECT xx)
{
    DbgPrint("Good Bye rdqx\n");
}
NTSTATUS  DriverEntry(
    PDRIVER_OBJECT driver,//就像是dll加载的时候会返回一个dllbase,driver指向了一个结构体	
    PUNICODE_STRING reg_path//你的驱动被注册在哪一个路径之下
)
{
    driver->DriverUnload = my_unload;//安全卸载函数
    DbgPrint("redqx: %wZ\n", reg_path);
    return 0;
}
```



首先建一个工程

![image-20230922132034073](./img/image-20230922132034073.png)

然后删除这些东西

![image-20230922132133259](./img/image-20230922132133259.png)

然后配置项目属性

![image-20230922132244239](./img/image-20230922132244239.png)

然后就可以直接生成了

![image-20230922132310455](./img/image-20230922132310455.png)



然后我们尝试在xp中加载一个文件

![image-20230922132418069](./img/image-20230922132418069.png)

把文件拖进工具中

然后点击register

然后点击run

卸载的流程应该是: stop,unregister吧



可以看到注册成功,

![image-20230922132508517](./img/image-20230922132508517.png)

然后run成功,,windbg停下来

![image-20230922132540044](./img/image-20230922132540044.png)

然后你就g一下运行呗

首先输出

![image-20230922132638809](./img/image-20230922132638809.png)

我们尝试卸载

先stop,然后unrgister

![image-20230922132713044](./img/image-20230922132713044.png)

可以看到输出

![image-20230922132734553](./img/image-20230922132734553.png)







# WDF和WDM有什么区别

驱动分为2种,一种是NT的,一种是WDM的

NT: 较早的,一些ntddk.h都是基于nt的驱动, 属于冷插拔,,,比如插入一个鼠标的NT驱动,,需要重启才可以使用

WDM: 热插拔,即插即用



不管哪一种,,,和我们搞安全都无所谓...

真的无所谓?....



![image-20231019104723562](img/image-20231019104723562.png)

在我们的电脑卡死的时候,,长按电源键

会发起了中断请求,,,因为该中断请求属于最高等级

尽管我们电脑卡死了,,,但是我们仍然可以关机



在KPCR.Irql   中,就存放了我们的请求等级

```
0: kd> dt _KPCR
nt!_KPCR
   +0x000 NtTib            : _NT_TIB
   +0x01c SelfPcr          : Ptr32 _KPCR
   +0x020 Prcb             : Ptr32 _KPRCB
   +0x024 Irql             : UChar
   +0x028 IRR              : Uint4B
   +0x02c IrrActive        : Uint4B
   +0x030 IDR              : Uint4B
   +0x034 KdVersionBlock   : Ptr32 Void
   +0x038 IDT              : Ptr32 _KIDTENTRY
   +0x03c GDT              : Ptr32 _KGDTENTRY
   +0x040 TSS              : Ptr32 _KTSS
   +0x044 MajorVersion     : Uint2B
   +0x046 MinorVersion     : Uint2B
   +0x048 SetMember        : Uint4B
   +0x04c StallScaleFactor : Uint4B
   +0x050 DebugActive      : UChar
   +0x051 Number           : UChar
   +0x052 Spare0           : UChar
   +0x053 SecondLevelCacheAssociativity : UChar
   +0x054 VdmAlert         : Uint4B
   +0x058 KernelReserved   : [14] Uint4B
   +0x090 SecondLevelCacheSize : Uint4B
   +0x094 HalReserved      : [16] Uint4B
   +0x0d4 InterruptMode    : Uint4B
   +0x0d8 Spare1           : UChar
   +0x0dc KernelReserved2  : [17] Uint4B
   +0x120 PrcbData         : _KPRCB

```





# Hello World



```c
#include<ntddk.h>
void my_unload(PDRIVER_OBJECT xx)
{
    DbgPrint("Good Bye rdqx\n");
}
NTSTATUS  DriverEntry(
    PDRIVER_OBJECT driver,//就像是dll加载的时候会返回一个dllbase,driver指向了一个结构体	
    PUNICODE_STRING reg_path//你的驱动被注册在哪一个路径之下
)
{
    driver->DriverUnload = my_unload;//安全卸载函数
	DbgPrint("redqx: %wZ\n", reg_path);
	DbgPrint("p_driver: %X\n", p_driver);
	return STATUS_SUCCESS;
}
```

 

![image-20231019121438150](img/image-20231019121438150.png)

![image-20231019121456321](img/image-20231019121456321.png)

然后去看看那个结构体

```
0: kd> dt _DRIVER_OBJECT 89EB3030
ntdll!_DRIVER_OBJECT
   +0x000 Type             : 0n4
   +0x002 Size             : 0n168
   +0x004 DeviceObject     : (null) 
   +0x008 Flags            : 0x12
   +0x00c DriverStart      : 0xbabf8000 Void
   +0x010 DriverSize       : 0x6000
   +0x014 DriverSection    : 0x89b22df8 Void
   +0x018 DriverExtension  : 0x89eb30d8 _DRIVER_EXTENSION
   +0x01c DriverName       : _UNICODE_STRING "\Driver\MyDriver3"
   +0x024 HardwareDatabase : 0x8067e260 _UNICODE_STRING "\REGISTRY\MACHINE\HARDWARE\DESCRIPTION\SYSTEM"
   +0x028 FastIoDispatch   : (null) 
   +0x02c DriverInit       : 0xbabfc000     long  MyDriver3!GsDriverEntry+0
   +0x030 DriverStartIo    : (null) 
   +0x034 DriverUnload     : 0xbabf9040     void  MyDriver3!Driver_UnLoad+0
   +0x038 MajorFunction    : [28] 0x804f5552     long  nt!IopInvalidDeviceRequest+0

```

