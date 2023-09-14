# 前言

参考链接 https://www.cnblogs.com/LyShark/p/13525917.html



x86和x64结构不一样

这里以x64为例子



```c
typedef struct _IMAGE_TLS_DIRECTORY32
{
    DWORD   StartAddressOfRawData;	//文件主线程局部存储tls的起点,VA
    DWORD   EndAddressOfRawData;	//文件主线程局部存储tls的终点,VA
    DWORD   AddressOfIndex;          //tls索引位置
    DWORD   AddressOfCallBacks;     //指针数组,成员是回调的函数,以NULL结尾
    DWORD   SizeOfZeroFill;			//填充0的个数,
    DWORD   Characteristics;		//保留 0xCCCCCCCC
} IMAGE_TLS_DIRECTORY32;
```

这里主要讲TLS回调函数



# 动态线程局部存储技术

有一个TLS数组在结构体PEB中,

arr[64]

同时有一个8字节,64bit,记录了数组成员每一位是否被使用

```
DWORD TlsAlloc(VOID)();
```

函数成功返回一个索引值值,失败返回-1

一旦函数返回成功,对应8字节那个bit位就变为1



```c
DWORD TlsGetValue(DWORD index);
```

成功就返回对应的值,失败就返回0,当然里arr[]的值本来就可以是0,所以失败还需进行getlasterror进一步判断



```c
BOOL SetTlsValue(DWORD index,LPVOID lpTlsValue)
```

失败返回0,成功不返回0



```c
BOOL TlsFree(DWORD index);
```

失败返回0,成功不返回0,并把对应位置为0



如果如果arr[]装的是你自己申请的内存,那么也得是你自己释放,而不是tlsfree释放

```c
#include<Windows.h>
#include <stdio.h>

DWORD WINAPI MY_ThreadProc(LPVOID);
DWORD32 MY_GetValue(DWORD32);

int main()
{
    HANDLE gh_tmp=0;
    gh_tmp = CreateThread(0, 0, MY_ThreadProc, 0, 0, 0);
    WaitForSingleObject(gh_tmp, 0xffffffff);
    system("pause");
    return;
}

DWORD WINAPI MY_ThreadProc(LPVOID lp_Param)
{
    DWORD32 dw_tmp = 0, index = 0;
    index = TlsAlloc();
    TlsSetValue(index, (LPVOID)0x1234);
    dw_tmp = TlsGetValue(index);
    printf("%08X\n", dw_tmp);
    TlsFree(index);
    return 0;
}
```



# 静态线程局部存储技术



就是手动在源代码写入数据

```c
#pragma comment(linker, "/INCLUDE:__tls_used")

// TLS变量
__declspec (thread) int  g_nNum = 0x64;
__declspec (thread) char g_szStr[] = "TLS g_nNum = 0x%p ...\r\n";

//如果要写入函数的话
// 注册TLS回调函数，".CRT$XLB"
#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback[] = { TLS_A, TLS_B};
#pragma data_seg()
```



# TLS回调函数



```c
void NTAPI TLS_B(PVOID DllHandle, DWORD Reason, PVOID Red);
```

参数解释:

1. Reserved:预留，为0。
2. Reason:调用该回调函数的时机
3. DllHandle:DLL的句柄。



标准的tls,了解一下

```c
void NTAPI TLS_B(PVOID DllHandle, DWORD Reason, PVOID Red)
{
    switch (Reason)
    {
        case DLL_PROCESS_ATTACH:
            printf("Process Attach\n");
            break;
        case DLL_PROCESS_DETACH:
            printf("Process Detach\n");
            break;
        case DLL_THREAD_ATTACH:
            printf("Thread Attach\n");
            break;
        case DLL_THREAD_DETACH:
            printf("Thread Detach\n");
            break;
        default:
            break;
    }
    return;
}
```

![Untitled](img/7dd3496d4e184434a98edc11de043e96Untitled15.png)

在主线程加载的时候,主线程会把所有tls调用个遍,调用的理由Process Attach

奇怪的是,我的主线程结束的时候,并没有调用tls,

主线程如果创建了子线程,那么也会调用所有tls函数,调用理由Thread Attach

子线程结束的时候,那么也会调用tls函数,调用理由Thread Detach

写入tls函数

```c
#include <Windows.h>
#include <stdio.h>
#pragma comment(linker, "/INCLUDE:__tls_used")


VOID _stdcall TLS_A(PVOID DllHandle, DWORD Reason, PVOID Reserved);
VOID _stdcall TLS_B(PVOID DllHandle, DWORD Reason, PVOID Reserved);
//然后巴拉巴拉注册
#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback[] = { TLS_B, TLS_A };
#pragma data_seg()

//然后巴拉巴拉
int main()
{
	printf("hello i am main\n");
	return 0;
}
VOID _stdcall TLS_A(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
	printf("hello i am TLS A\n");
	return ;
}
VOID _stdcall TLS_B(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
	printf("hello i am TLS B\n");
	return ;
}
```

另外一种奇奇怪怪的写入

```c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include < Windows.h>

#pragma section(".CRT$XLY", long, read)
__declspec(thread) int var = 0xDEADBEEF;

VOID _stdcall TlsCallback(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
    var = 0xB15BADB0; // Required for TLS Callback call
    if (IsDebuggerPresent())
    {
        MessageBoxA(NULL, "Stop debugging program!", "Error", MB_OK | MB_ICONERROR);
        TerminateProcess(GetCurrentProcess(), 0xBABEFACE);
    }
}
__declspec(allocate(".CRT$XLY")) PIMAGE_TLS_CALLBACK g_tlsCallback = TlsCallback;

int main()
{
    puts("Hello World");
    return 0;
}
```
