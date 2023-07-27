



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



windows加载dll

1), 未指定目录加载, 首先会找系统目录的dll,找不到就去当前路径

2), 指定目录加载,找不到就g

ps: 一个exe加载的dll并不止是只有导入表那些,比如ntdll,...

这些dll的加载可能是通过其他dll模块去主动再加载的

比如dll里面再调用Loadlirbary





对于那些把DLL路径写死的,就不是很好解决

如何判断它有没有把某些DLL路径写死? 

方法1:

你把exe单独拖到任意路径,

如果看到exe仍然可以运行

那么大概说明该DLL的路径不是写死的



ps: 如我是开发着,在第一次安装的时候,我就会记录当前文件位置环境并写死到全局的环境变量

就算把exe拖到其他地方去,我就读取环境变量,一样可以成功,因为环境变量的位置是不变的



方法2:

对LoadLibraryA/W下断点,看写入的路径是不是一个绝对路径