# 记事本-WriteFile() API钩取

[mycode/Re/API_HOOK/notepad_writefile_api_hook at e1e256b4b417a61447728d10f37d328b620c25a0 · redqx/mycode](https://github.com/redqx/mycode/tree/e1e256b4b417a61447728d10f37d328b620c25a0/Re/API_HOOK/notepad_writefile_api_hook)

# 简单介绍

采用的技术

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-WriteFile()%20API%E9%92%A9%E5%8F%96%200c1655d9c71b41038af96fd1a2ebd764/Untitled.png)

基于调试器的原理,所以就不多说了

只是简单的回忆一些东西

调试的事件类型 dwDebugEventCode

```c
#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9
```

上面列出的调试事件中，与调试相关为`EXCEPTION_DEBUG_EVENT`，

下面是与相关的对应异常列表ID

```c
EXCEPTION_ACCESS_VIOLATION
EXCEPTION_ARRAY_BOUNDS_EXCEEDED
EXCEPTION_BREAKPOINT
EXCEPTION_DATATYPE_MISALIGNMENT
EXCEPTION_FLT_DENORMAL_OPERAND
EXCEPTION_ELT_DIVIDE_BY_ZERO
EXCEPTION_ELT_INEXACT_RESULT
EXCEPTION_ELT_INVALID_OPERATION
EXCEPTION_ELT_OVERFLOW
EXCEPTION_ELT_STACK_CHECK
EXCEPTION_ELT_UNDERFLOW
EXCEPTION_ILLEGAL_INSTRUCTION
EXCEPTION_IN_PAGE_ERROR
EXCEPTION_INT_DIVIDE_BY_ZERO
EXCEPTION_INT_OVERFLOW
EXCEPTION_INVALID_DISPOSITION
EXCEPTION_NONCONTINUABLE_EXCEPTION
EXCEPTION_PRIV_INSTRUCTION
EXCEPTION_SINGLE_STEP
EXCEPTION_STACK_OOVERFLOW
```

调试器必须处理的是EXCEPTION_BREAKPOINT异常。断点对应的汇编指令为INT3, IA-32指令为0xCC

调试器实现断点的方法非常简单，找到要设置断点的代码在内存中的起始地址，

只要把1个 字节修改为0xCC就可以了。想继续调试时，再将它恢复原值即可。通过调试钩取API的技术就是利用了断点的这种特性

具体流程如下：

1. 对想钩取的进程进行附加操作，使之成为被调试者；
2. “钩子”：将 API 起始地址的第一个字节修改为 0xCC；
3. 调用相应的 API 时，控制权转移到调试器；
4. 执行需要的操作（操作参数、返回值等）；
5. 脱钩：将 0xCC 恢复原值（为了正常运行 API）；
6. 运行相应的API（无0xCC的正常状态）；
7. “钩子”：再次修改为 0xCC （为了继续钩取）；
8. 控制权返还被调试者

以上介绍的就是最简单的情形，在此基础上可以有多种变化。

既可以不调用原始的 API ，也可以调用用户提供的客户 API ；

可以只钩取一次，也可以钩取多次。

实际应用时，根据需要适当调整即可。

WriteFile()定义（出处：MSDN）如下：

# 注入器分析-代码分析

## 相关结构体&函数介绍

### DEBUG_EVENT

```c
typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;//事件发生的类型,比如EXCEPTION_DEBUG_EVENT
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO Exception;
        CREATE_THREAD_DEBUG_INFO CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO ExitThread;
        EXIT_PROCESS_DEBUG_INFO ExitProcess;
        LOAD_DLL_DEBUG_INFO LoadDll;
        UNLOAD_DLL_DEBUG_INFO UnloadDll;
        OUTPUT_DEBUG_STRING_INFO DebugString;
        RIP_INFO RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;
```

前面提到了共有9种调试事件。`DEBUG_EVENT.dwDebugEventCode`成员会被设置为九种事件中的一种，

根据相关事件的种类，也会设置适当的`DEBUG_EVENT.u(union)成员（DEBUG_EVENT.u共用体成员内部也有九个结构体组成，它们对应事件种类的个数）

如果发生异常事件时，

dwDebugEventCode 成员会被设置为 EXCEPTION_DEBUG_EVENT ，

 u.Exception 结构体也会得到设置。

### ContinueDebugEvent

```c
WINBASEAPI BOOL APIENTRY ContinueDebugEvent(
    _In_ DWORD dwProcessId,
    _In_ DWORD dwThreadId,
    _In_ DWORD dwContinueStatus
    );
```

ContinueDebugEvent() API 的最后一个参数 dwContinueStatus 的值为 DGBG_CONTINUE 或 DBG_EXCEPTION_NOT_HANDLED 。

若处理正常，则其值设置为 DBG_CONTINUE ；

若无法处理，或希望交付给应用程序的自己的异常处理函数 中 处理，则其值应该为 DBG_EXCEPTION_NOT_HANDLED

## main

```c
int main(int argc, char* argv[])
{
    DWORD dwPID = 0;
    HANDLE hProcess;
    if (0)
    {
        if (argc != 2)
        {
            printf("\n USAGE  : %s <pid>\n", argv[0]);
            return 1;
        }
        // code injection
        dwPID = (DWORD)atol(argv[1]);
    }
    else
    {
        dwPID = FindProcessID(TEXT("notepad.exe"));
    }

    if (dwPID != -1)
    {
        //获取目标进程架构 因为hook x86和x64的不一样
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
        IsWow64Process(hProcess, &is_x86);
        CloseHandle(hProcess);

        if (!DebugActiveProcess(dwPID))//让目标进程被我调试
        {
            printf("DebugActiveProcess(%d) failed!!!\nError Code = %d\n", dwPID, GetLastError());
            return 1;
        }
        debugWhile();//进入一个消息循环
    }
    return 0;
}
```

`DebugActiveProcess` 是Windows操作系统提供的一个函数，

用于启动一个进程的调试会话。它可以将调试器附加到指定的进程上，允许开发人员监视和控制该进程的执行。

通过调用 `DebugActiveProcess` 函数，可以实现以下功能：

1. **远程调试**: 可以通过将调试器连接到远程计算机上的进程，实现远程调试功能。这对于排查和解决远程计算机上的问题非常有用。
2. **代码调试**: 调试器可以附加到正在运行的进程中，允许开发人员跟踪程序的执行、检查变量的值、运行和停止程序，并进行其他与调试相关的操作。
3. **异常处理**: 调试器可以拦截和处理进程中的异常。当进程遇到错误或异常时，调试器可以捕获并提供更多的上下文信息，以帮助开发人员诊断问题。
4. **逆向工程**: `DebugActiveProcess` 函数也被用于逆向工程和安全测试。它允许将调试器附加到目标进程，并分析其内部运行机制，以进行代码审计或漏洞研究。

需要注意的是，使用 `DebugActiveProcess` 函数需要相应的权限，并且在某些情况下，可能会受到一些安全策略的限制。

在正常情况下，它主要用于开发和调试目的，以及在合法和授权的环境中使用。

如果不正确地使用或滥用 `DebugActiveProcess` 函数，可能会导致潜在的安全风险和违法行为。

因此，在使用这个函数时，请确保遵守适用的法律法规和道德准则

然后说一下main函数的一个流程

首先获取目标进程的PID

然后判断该进程是什么架构x64还是x86

然后开始附加调试目标进程

进入一个和调试相关的消息循环

## debugWhile

类似一个消息循环

不断的去处理被调试的进程发生的事件(异常,…)

```c
/*
* 名字:debugWhile
* 参数:无参数
* 作用: 不断的处理被调试进程的事件
*/
void debugWhile()
{
    DEBUG_EVENT de;
    DWORD dwContinueStatus;

    //等待被调试的notepad发生一个事件,然后把信号发过来
    while (WaitForDebugEvent(&de, INFINITE))//不断的去进行一个调试器的消息循环
    {
        dwContinueStatus = DBG_CONTINUE;//被调试者选择 继续执行

        if (CREATE_PROCESS_DEBUG_EVENT == de.dwDebugEventCode)//调试进程生成或者附加事件,只会发生一次
        {
            write0xCC(&de);//对Writefile函数写入0xcc
        }
        else if (EXCEPTION_DEBUG_EVENT == de.dwDebugEventCode)//被调试的notepad发生了异常事件,通常是0xCC
        {
            if (handler_0xCC(&de))//除以0xcc的异常
            {
                continue;
            }
        }
        // 被调试进程终止
        else if (EXIT_PROCESS_DEBUG_EVENT == de.dwDebugEventCode)
        {
            // 被调试者已经终止,调试器也后续的发生终止break
            break;
        }
        // 调速器继续然被调试的程序运行
        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus);//调速器继续运行
    }
}
```

它处理的事件也就分为3种

1), 刚开始调试的时候 CREATE_PROCESS_DEBUG_EVENT 

2),发生0xCC异常的时候 EXCEPTION_DEBUG_EVENT

3), 目标进程退出的时候 EXIT_PROCESS_DEBUG_EVENT

这些事件分别对应了一些处理函数

首先这里小小的说一句

x86和x64调用的dll是不一样的

比如

x86调用的dll在目录SysWOW64下

x64调用的dll在目录system32下

所以在API钩取的时候,load的dll不要搞错了

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-WriteFile()%20API%E9%92%A9%E5%8F%96%200c1655d9c71b41038af96fd1a2ebd764/Untitled%201.png)

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-WriteFile()%20API%E9%92%A9%E5%8F%96%200c1655d9c71b41038af96fd1a2ebd764/Untitled%202.png)

### write0xCC

其实就是在进程被附加调试的时候

去hook writefile的API

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-WriteFile()%20API%E9%92%A9%E5%8F%96%200c1655d9c71b41038af96fd1a2ebd764/Untitled%203.png)

这里就是去往writefile

hook的具体操作就是修改第一个0xFF字节为0xCC

于是后面再调用相关API的时候就会触发这个int3异常

```c
/*
* 函数名: write0xCC
* 函数参数:
*  - 参数1: 不知道具体含义
* 函数功能: hook writefile的API,方便后面触发异常
*/
BOOL write0xCC(LPDEBUG_EVENT pde)
{
    //下面这2个地址也可以在其它进程获取,而不是写死
    if (is_x86)
    {
        lp_orgWriteFile = (LPVOID)0x77533C50;//API jmp write file
        //77533C50
    }
    else
    {
        //00007FFFA7F25750 
        lp_orgWriteFile = (LPVOID)0x00007FFFA7F25750;//API jmp write file
    }

    // API Hook - WriteFile()
    memcpy(&g_cpdi, &pde->u.CreateProcessInfo, sizeof(CREATE_PROCESS_DEBUG_INFO));
    ReadProcessMemory(g_cpdi.hProcess, lp_orgWriteFile, &asm_old, 1, NULL);   //把原始的字节保存到asm_old
    WriteProcessMemory(g_cpdi.hProcess, lp_orgWriteFile, &int3, 1, NULL);    //往对应的地址那里写入0xCC

    return TRUE;
}
```

### handler_0xCC

```c
BOOL WriteFile(
  [in]                HANDLE       hFile,
  [in]                LPCVOID      lpBuffer, //数据缓冲区指针
  [in]                DWORD        nNumberOfBytesToWrite,//为要写的字节数
  [out, optional]     LPDWORD      lpNumberOfBytesWritten,
  [in, out, optional] LPOVERLAPPED lpOverlapped
);
```

发生int3异常的不一定是Write file,所以我们还得做一些判断

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-WriteFile()%20API%E9%92%A9%E5%8F%96%200c1655d9c71b41038af96fd1a2ebd764/Untitled%204.png)

然后,让我很意外的是x86和x64用的是同一个 `CONTEXT ctx = { 0 };` 上下文结构体

之前我还分开用, x64用x64的,x86用x86的,然后捣鼓了很久没成功,发现根本没必要去在意

微软已经给我们处理好了

另外就是0xCC的异常不知有 `0x80000003`  还有 `0x4000001F` 

真意外

其中0x4000001F是针对于x86的0xCC异常

0x80000003 是针对于x64的0xCC异常

也困扰了我很久的

如果是我们writefile触发的0xCC异常

我们就去读取相关上下文结构体

然后做一个数据截取和处理

然后再写入进去

但是还有一点需要注意

x86用的栈传递参数

x64是寄存器传参

因为这个东西还会引发一个问题

就是怎么获取线程上下文

对于x86的,我们要用esp,eip所以就根据

```c
ctx.ContextFlags = CONTEXT_CONTROL;//包括控制寄存器 (RIP, RSP, RFLAGS 等) 的上下文信息。
GetThreadContext(g_cpdi.hThread, (LPCONTEXT)&ctx);
```

对于x64,我们要获取rip,rdx,r8,是所以

```c
// #2. Thread Context 获取线程上下文
ctx.ContextFlags = CONTEXT_INTEGER| CONTEXT_CONTROL; //包括整数寄存器 (RAX, RBX, RCX, RDX 等) 的上下文信息。
GetThreadContext(g_cpdi.hThread, (LPCONTEXT)&ctx);
```

这东西也困扰了我很久,如果你没有访问某些寄存器的权限

却`SetThreadContext(g_cpdi.hThread, (LPCONTEXT)&ctx);` 那么就会引发一些错误的东西

比如你没有rip的修改权限(没有在ctx.ContextFlags关联rip)

然后你修改了rip,然后您SetThreadContext(g_cpdi.hThread, (LPCONTEXT)&ctx)

那么修改是无效的

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-WriteFile()%20API%E9%92%A9%E5%8F%96%200c1655d9c71b41038af96fd1a2ebd764/Untitled%205.png)

最后处理就是

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-WriteFile()%20API%E9%92%A9%E5%8F%96%200c1655d9c71b41038af96fd1a2ebd764/Untitled%206.png)

其中你会发现那里有一个sleep()是干嘛的?

首先我们应该可以理解后面有一个继续挂钩的操作

ContinueDebugEvent 让进程继续运行(脱钩)

WriteProcessMemory 挂钩

如果这个函数之间没有间隙可言

也就是目标进程在ContinueDebugEvent后,还没有开始执行这块间隙之间,就被再次WriteProcessMemory 挂钩的话

目标进程就会再次触发一个0xCC异常,停滞不前

如果我们sleep了一下

也就让目标进程ContinueDebugEvent 完全结束

跑完writefile

然后我们再对writefile进行一个挂钩