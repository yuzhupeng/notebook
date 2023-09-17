



https://blog.csdn.net/evileagle/article/details/12718845

和dll加载有关

以前dll的加载是,有很多dll干活,把所有dll都喊进屋,到谁干活谁就出来, ps:屋子可能不太好装下怎么多人

延迟导入就是准备很多dll干活,不先把他们领进门,让他们所有人在外面等着,干活时,喊谁,谁就进来

延迟加载导入是一种合理利用进程加载机制提高进程加载效率的技术，

使用延迟加载导入能跳过加载前对引入函数的检测及加载后对IAT的修正，

从而避免出现诸如“无法找到组件”的错误提示，提高程序的适应性。

及时即用?

![Untitled](img/7dd3496d4e184434a98edc11de043e96Untitled11.png)

```
typedef struct _IMAGE_DELAYLOAD_DESCRIPTOR
{
    union {
        DWORD AllAttributes;// 属性,说必须为0
        struct
        {
            DWORD RvaBased : 1;             // Delay load version 2
            DWORD ReservedAttributes : 31;
        } DUMMYSTRUCTNAME;
    } Attributes;

    DWORD DllNameRVA;                       // 指向dll的名字,好像是一个VA地址
    DWORD ModuleHandleRVA;                  // 指向dll的模块句柄
    DWORD ImportAddressTableRVA;            // IAT
    DWORD ImportNameTableRVA;               // INT
    DWORD BoundImportAddressTableRVA;       // 绑定该表的RVA 可选
    DWORD UnloadInformationTableRVA;        // 卸载该表的RVA 可选
    DWORD TimeDateStamp;                    // 0 if not bound,
    // Otherwise, date/time of the target DLL

} IMAGE_DELAYLOAD_DESCRIPTOR, *PIMAGE_DELAYLOAD_DESCRIPTOR;
```

不使用延迟加载

```
ml -c -coff He1loworld.asm
link -subsystem : windows Hellowor1d.obj
He1loworld
```

使用延迟加载

```
ml -c -coff Helloworld.asm
link -subsystem:windows -delayload:MyD11.dll delayimp.lib HelloWorld.obj
Helloworld
```

那么link -subsystem:windows -delayload:MyD11.dll delayimp.lib HelloWorld.obj干了什么?

首先，将一个函数_delayLoadHelper嵌入PE文件的可执行模块。

其次，从可执行模块的导入表部分删除MyDl.dll 及相关信息，这样，当进程初始化的时候，

操作系统的加载程序就不会显式加载该动态链接库了。

最后，在PE中把刚才删除的相关信息重新构造好，以便告诉_delayLoadHelper哪些函数是从MyDll.dll中导出的。

在调用API的时候

![Untitled](img/7dd3496d4e184434a98edc11de043e96Untitled12.png)

然后出现就出现了区别

不使用该技术就直接前往该dll

使用该技术,去往某个函数,然后LoadLibiary和GetProcessAddress,如果再次调用不再load和get

为什么使用了延迟导入表技术可以实现任何地方都可以轻松运行?

因为他把dll文件作为资源放进了自己那里,然后需要的时候再调用资源?

关于这个书中有代码讲解,我没有去实现,,,,QWQ,还是去写一下吧,万一有用呢

关于延迟导入的dll卸载,可以使用-delay:unload选项

这是一个可选项,具体深入还不知道