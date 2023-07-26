



# dllMain



```c
BOOL WINAPI DllMain(
   HINSTANCE hinstDLL, // Dll的ImageBase
   DWORD fdwReason, // 调用原因
   LPVOID lpvReserved // 表示一个保留参数
)
{
	switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
        default:
            break;
    }
    return TRUE;

}

```





其中fdwReason用来表示Dll被调用的状态，一共分为四种：

-   DLL_PROCESS_ATTACH  ==1 被进程加载 进程映射 LoadLibiary的时候
-   DLL_PROCESS_DETACH  ==0 被进程释放 进程卸载 FreeLibiary的时候
-   DLL_THREAD_ATTACH   ==2 被线程加载 线程映射
-   DLL_THREAD_DETACH   ==3 被线程释放 线程卸载



# 导出一个函数

使用 `__declspec(dllexport)`导出函数



```c
__declspec(dllexport) int add(int,int)
{
    //不写_declspec的话,你的函数就不会导出,也就是不会出现在导出表里面
	...
}
```

注意:动态库编译链接后, 也会有LIB文件,是作为动态库函数`映射`使用,与静态库`不完全相同`。



这种导出方式对于cpp文件来说,会出现换名的机制,woc

所以,我们就在前面加上一个 extern "C" 

```c
extern "C" __declspec(dllexport) void TestFuction( )
{
    return ;
}
```







# 加载和卸载

Freelibiary

每一个内核对象都拥有一个引用技术

代表对象被使用的次数

如果加载了10次dll,那么相应的也要卸载10次