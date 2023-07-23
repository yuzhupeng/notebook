# 逆向过程核心原理 CodeInject.cpp分析

[mycode/Re/Dll注入/代码注入 at f3bc146327151fed97bd119ae0ef8d11b948f1e2 · redqx/mycode](https://github.com/redqx/mycode/tree/f3bc146327151fed97bd119ae0ef8d11b948f1e2/Re/Dll注入/代码注入)

# 整体的一个流程

# main

```c
int main(int argc, char* argv[])
{
    DWORD dwPID = 0;

    if (argc != 2)
    {
        printf("\n USAGE  : %s <pid>\n", argv[0]);
        return 1;
    }

    // change privilege
    if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
        return 1;

    // code injection
    dwPID = (DWORD)atol(argv[1]);
    InjectCode(dwPID);

    return 0;
}
```

可以看出这是一个cmd执行

执行参数2 是要注入的进程的pid

然后是一个常规的提权操作

然后就是代码注入的东西

# InjectCode

因为代码注入需要传递变量过去

于是需要一个好的结构体去创建规划这些变量的布局

```c
typedef struct _THREAD_PARAM
{
    FARPROC pFunc[2];               // LoadLibraryA(), GetProcAddress()
    char    szBuf[4][128];          // "user32.dll", "MessageBoxA", "www.reversecore.com", "ReverseCore"
} THREAD_PARAM, * PTHREAD_PARAM;
```

因为传递了2和函数过去,所以用 FARPROC pFunc[2];

同时传递了4个字符串过去 ,所以用char    szBuf[4][128];

```c
BOOL InjectCode(DWORD dwPID)
{
    HMODULE         hMod = NULL;
    THREAD_PARAM    param = { 0, };
    HANDLE          hProcess = NULL;
    HANDLE          hThread = NULL;
    LPVOID          pRemoteBuf[2] = { 0, };
    DWORD           dwSize = 0;

    hMod = GetModuleHandle(TEXT("kernel32.dll"));

    // set THREAD_PARAM

    param.pFunc[0] = GetProcAddress(hMod, "LoadLibraryA");//没有w版的区分
    param.pFunc[1] = GetProcAddress(hMod, "GetProcAddress");
    strcpy(param.szBuf[0], "user32.dll");
    strcpy(param.szBuf[1], "MessageBoxA");
    strcpy(param.szBuf[2], "https://redqx.vercel.app/");
    strcpy(param.szBuf[3], "blog");

    // Open Process
    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS,   // dwDesiredAccess
        FALSE,                // bInheritHandle
        dwPID)))             // dwProcessId
    {
        wprintf(L"OpenProcess() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }

    // Allocation for THREAD_PARAM
    dwSize = sizeof(THREAD_PARAM);
    if (!(pRemoteBuf[0] = VirtualAllocEx(hProcess,          // hProcess
        NULL,                 // lpAddress
        dwSize,               // dwSize
        MEM_COMMIT,           // flAllocationType
        PAGE_READWRITE)))    // flProtect
    {
        wprintf(L"VirtualAllocEx() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess,                       // hProcess
        pRemoteBuf[0],                  // lpBaseAddress
        (LPVOID)&param,                 // lpBuffer
        dwSize,                         // nSize
        NULL))                         // [out] lpNumberOfBytesWritten
    {
        wprintf(L"WriteProcessMemory() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }

    // Allocation for ThreadProc()
    dwSize = (DWORD)InjectCode - (DWORD)ThreadProc;
    if (!(pRemoteBuf[1] = VirtualAllocEx(hProcess,          // hProcess
        NULL,                 // lpAddress
        dwSize,               // dwSize
        MEM_COMMIT,           // flAllocationType
        PAGE_EXECUTE_READWRITE)))    // flProtect
    {
        wprintf(L"VirtualAllocEx() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess,                       // hProcess
        pRemoteBuf[1],                  // lpBaseAddress
        (LPVOID)ThreadProc,             // lpBuffer
        dwSize,                         // nSize
        NULL))                         // [out] lpNumberOfBytesWritten
    {
        wprintf(L"WriteProcessMemory() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }
    //hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);//创建一个远程线程,指定了线程处理函数,线程参数(参数的地址是刚才在目标进程分配的)
    if (!(hThread = CreateRemoteThread(
        hProcess,            // hProcess
        NULL,                // lpThreadAttributes
        0,                   // dwStackSize
        (LPTHREAD_START_ROUTINE)pRemoteBuf[1],     // dwStackSize
        pRemoteBuf[0],       // lpParameter
        0,                   // dwCreationFlags
        NULL)))             // lpThreadId
    {
        wprintf(L"CreateRemoteThread() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}
```

首先是准备好要用到的一些变量

```c
    hMod = GetModuleHandle(TEXT("kernel32.dll"));

    // set THREAD_PARAM

    param.pFunc[0] = GetProcAddress(hMod, "LoadLibraryA");//没有w版的区分
    param.pFunc[1] = GetProcAddress(hMod, "GetProcAddress");
    strcpy(param.szBuf[0], "user32.dll");
    strcpy(param.szBuf[1], "MessageBoxA");
    strcpy(param.szBuf[2], "https://redqx.vercel.app/");
    strcpy(param.szBuf[3], "blog");
```

然后打开进程

```c
    // Open Process
    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS,   // dwDesiredAccess
        FALSE,                // bInheritHandle
        dwPID)))             // dwProcessId
    {
        wprintf(L"OpenProcess() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }
```

然后在远程进程分配内存

把用到的变量写过去

```c
    // Allocation for THREAD_PARAM
    dwSize = sizeof(THREAD_PARAM);
    if (!(pRemoteBuf[0] = VirtualAllocEx(hProcess,          // hProcess
        NULL,                 // lpAddress
        dwSize,               // dwSize
        MEM_COMMIT,           // flAllocationType
        PAGE_READWRITE)))    // flProtect
    {
        wprintf(L"VirtualAllocEx() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess,                       // hProcess
        pRemoteBuf[0],                  // lpBaseAddress
        (LPVOID)&param,                 // lpBuffer
        dwSize,                         // nSize
        NULL))                         // [out] lpNumberOfBytesWritten
    {
        wprintf(L"WriteProcessMemory() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }
```

再次分配内存,再次写过去

这不过这一次写过去的是代码(字节码),而不是变量

```c
// Allocation for ThreadProc()
    dwSize = (DWORD)InjectCode - (DWORD)ThreadProc;
    if (!(pRemoteBuf[1] = VirtualAllocEx(hProcess,          // hProcess
        NULL,                 // lpAddress
        dwSize,               // dwSize
        MEM_COMMIT,           // flAllocationType
        PAGE_EXECUTE_READWRITE)))    // flProtect
    {
        wprintf(L"VirtualAllocEx() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess,                       // hProcess
        pRemoteBuf[1],                  // lpBaseAddress
        (LPVOID)ThreadProc,             // lpBuffer
        dwSize,                         // nSize
        NULL))                         // [out] lpNumberOfBytesWritten
    {
        wprintf(L"WriteProcessMemory() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }
```

然后就在远程执行那块代码

```c
//hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);//创建一个远程线程,指定了线程处理函数,线程参数(参数的地址是刚才在目标进程分配的)
    if (!(hThread = CreateRemoteThread(
        hProcess,            // hProcess
        NULL,                // lpThreadAttributes
        0,                   // dwStackSize
        (LPTHREAD_START_ROUTINE)pRemoteBuf[1],     // dwStackSize
        pRemoteBuf[0],       // lpParameter
        0,                   // dwCreationFlags
        NULL)))             // lpThreadId
    {
        wprintf(L"CreateRemoteThread() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }
```

线程函数就是刚才传递过去的,在远程进程已经存在了

线程函数的参数也传递过去了,在远程进程也存在

最后就等待远程线程执行完毕,关闭句柄

```c
WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hProcess);
```

# 实践过程分析-注入notepad.exe

遇到了一些问题

首先说一下很久以前遇到的问题

比如我们在调用一个函数的时候

![Untitled](%E9%80%86%E5%90%91%E8%BF%87%E7%A8%8B%E6%A0%B8%E5%BF%83%E5%8E%9F%E7%90%86%20CodeInject%20cpp%E5%88%86%E6%9E%90%207ca131bbe8c34186b0667fb3d4b40b4d/Untitled.png)

实际call进入

![Untitled](%E9%80%86%E5%90%91%E8%BF%87%E7%A8%8B%E6%A0%B8%E5%BF%83%E5%8E%9F%E7%90%86%20CodeInject%20cpp%E5%88%86%E6%9E%90%207ca131bbe8c34186b0667fb3d4b40b4d/Untitled%201.png)

按照道理来说,call进入应该是

![Untitled](%E9%80%86%E5%90%91%E8%BF%87%E7%A8%8B%E6%A0%B8%E5%BF%83%E5%8E%9F%E7%90%86%20CodeInject%20cpp%E5%88%86%E6%9E%90%207ca131bbe8c34186b0667fb3d4b40b4d/Untitled%202.png)

反正..原理啥的不知道

只是认为这是visual stdio 的一个编译特性

知道就可以了

正是因为这个东西,所以会导致原来<<逆向工程核心原理>>的那个代码失败

```c
    dwSize = (DWORD)InjectCode - (DWORD)ThreadProc;
    if (!(pRemoteBuf[1] = VirtualAllocEx(hProcess,          // hProcess
        NULL,                 // lpAddress
        dwSize,               // dwSize
        MEM_COMMIT,           // flAllocationType
        PAGE_EXECUTE_READWRITE)))    // flProtect
    {
        wprintf(L"VirtualAllocEx() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess,                       // hProcess
        pRemoteBuf[1],                  // lpBaseAddress
        (LPVOID)ThreadProc,             // lpBuffer
        dwSize,                         // nSize
        NULL))                         // [out] lpNumberOfBytesWritten
    {
        wprintf(L"WriteProcessMemory() fail : err_code = %d\n", GetLastError());
        return FALSE;
    }
```

他的 dwSize = (DWORD)InjectCode - (DWORD)ThreadProc;

而地址 InjectCode,ThreadProc 分别代表的是2个函数的jmp地址

2个jmp地址之间的数据是什么??? 不知道

反正不是我们想要的可执行数据

所以我么应该取真正的地址

也就是jmp真正去往的地址

![Untitled](%E9%80%86%E5%90%91%E8%BF%87%E7%A8%8B%E6%A0%B8%E5%BF%83%E5%8E%9F%E7%90%86%20CodeInject%20cpp%E5%88%86%E6%9E%90%207ca131bbe8c34186b0667fb3d4b40b4d/Untitled%203.png)

真正的地址应该是下一条地址+偏移

```c
0x0000000140011357 + 0x0C09 = 0x140011f60
0x0000000140011082 + 0x0A0E = 0x140011a90
```

所以实际代码长度就是 0x140011f60-0x140011a90 = 0x4d0

所以我们就要通过代码的方式计算出这个地址,然后做一个运算

```c
/**
 * 函数名称：getReallCall
 * 函数功能：传递进来jmp的地址,然后传出函数的真正入口地址
 * 输入参数：
 *   - 参数1：jmp xx 的地址
 * 返回值：函数真正的地址
 * 函数的前提: 无
 */
BYTE* getReallCall(BYTE* lp)
{
   return  *(DWORD*)(lp + 1) + (BYTE*)lp + 5;
}
```

在对远程进程内存开辟的时候也要注意这个问题

```c
fs = getReallCall(ThreadProc);
fe = getReallCall(InjectCode);
dwSize = fs - fe; //虽然是x64,单是这里用x86的dword也不是不可以
if (!(pRemoteBuf[1] = VirtualAllocEx(hProcess,          // hProcess
    NULL,                 // lpAddress
    dwSize,               // dwSize
    MEM_COMMIT,           // flAllocationType
    PAGE_EXECUTE_READWRITE)))    // flProtect
{
    wprintf(L"VirtualAllocEx() fail : err_code = %d\n", GetLastError());
    return FALSE;
}

if (!WriteProcessMemory(hProcess,                       // hProcess
    pRemoteBuf[1],                      // lpBaseAddress
    (LPVOID)fs,   // lpBuffer, 不是ThreadProc
    dwSize,                             // nSize
    NULL))                              // [out] lpNumberOfBytesWritten
{
    wprintf(L"WriteProcessMemory() fail : err_code = %d\n", GetLastError());
    return FALSE;
}
```

# 如何调试

用visual stdio 调试 codeinject

用ida64 调notepad.exx

每次visual stdio 使用VirtualAllocEx和WriteProcessMemory 我们都可以通过ida在notepad的进程中找到传递过来的内容

当visual stdio第二次把代码传过来的时候,我们就可以从ida中找到地址

传递过去的线程参数:

![Untitled](%E9%80%86%E5%90%91%E8%BF%87%E7%A8%8B%E6%A0%B8%E5%BF%83%E5%8E%9F%E7%90%86%20CodeInject%20cpp%E5%88%86%E6%9E%90%207ca131bbe8c34186b0667fb3d4b40b4d/Untitled%204.png)

传递过去的代码:

![Untitled](%E9%80%86%E5%90%91%E8%BF%87%E7%A8%8B%E6%A0%B8%E5%BF%83%E5%8E%9F%E7%90%86%20CodeInject%20cpp%E5%88%86%E6%9E%90%207ca131bbe8c34186b0667fb3d4b40b4d/Untitled%205.png)

然后打一个F2的断点,然后F9 visual stdio

iDA那边的notepad就会停止下来

当然你可以在IDA那边设置一个 断点于线程开

![Untitled](%E9%80%86%E5%90%91%E8%BF%87%E7%A8%8B%E6%A0%B8%E5%BF%83%E5%8E%9F%E7%90%86%20CodeInject%20cpp%E5%88%86%E6%9E%90%207ca131bbe8c34186b0667fb3d4b40b4d/Untitled%206.png)

当线程开始的时候

```c
__int64 __fastcall ntdll_RtlUserThreadStart(__int64 code_fuc, __int64 argv)
{
  unsigned int v3; // eax
  unsigned int v4; // eax

  if ( off_7FFFA9E7BFF0 )
    return sub_7FFFA9DB0BC0(0i64);
  v3 = sub_7FFFA9DB0BC0(argv);
  v4 = (ntdll_RtlExitUserThread)(v3);
  return (ntdll_NtTerminateProcess)(-1i64, v4);
}
```

参数1就是代码地址

参数2就是传递过来的参数

后面就开始运行线程函数了