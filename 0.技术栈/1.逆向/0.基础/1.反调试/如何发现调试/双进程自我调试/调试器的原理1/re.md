# 调试器的原理1

[恶意样本分析手册——反调试篇（上）](http://blog.nsfocus.net/anti-test-articles1/)

# ****启动被调试程序****

当我们要对一个程序进行调试时，首先要做的当然是启动这个程序，这要使用CreateProcess这个Windows API来完成。

```c
CreateProcess(
	TEXT("Path"),
	NULL,
	NULL,
	NULL,
	FALSE,
	DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,
	NULL,
	NULL,
	&si,
	&pi
)
```

CreateProcess的第六个参数使用了DEBUG_ONLY_THIS_PROCESS，这意味着调用CreateProcess的进程成为了调试器，而它启动的子进程成了被调试的进程。

除了DEBUG_ONLY_THIS_PROCESS之外，还可以使用DEBUG_PROCESS

两者的不同在于：DEBUG_PROCESS会调试被调试进程以及它的所有子进程，而DEBUG_ONLY_THIS_PROCESS只调试被调试进程，不调试它的子进程。

一般情况下我们只想调试一个进程，所以应使用后者。

建议在第六个参数中加上CREATE_NEW_CONSOLE标记。

因为如果被调试程序是一个控制台程序的话，调试器和被调试程序的输出都在同一个控制台窗口内，显得很混乱，

加上这个标记之后，被调试程序就会在一个新的控制台窗口中输出信息。如果被调试程序是一个窗口程序，这个标记没有影响

# ****调试循环****

****一个进程成为被调试进程之后，在完成了某些操作或者发生异常时，它会发送通知给调试器，然后将自身挂起，

直到调试器命令它继续执行。这有点像Windows窗口的消息机制

被调试进程发送的通知称为调试事件，DEBUG_EVENT结构体描述了调试事件的内容：

```c
typedef struct _DEBUG_EVENT {
  DWORD dwDebugEventCode;
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

其中有一个dwDebugEventCode

dwDebugEventCode描述了调试事件的类型，总共有9类调试事件：

| CREATE_PROCESS_DEBUG_EVENT | 创建进程之后发送此类调试事件，这是调试器收到的第一个调试事件。 |
| --- | --- |
| CREATE_THREAD_DEBUG_EVENT | 创建一个线程之后发送此类调试事件。 |
| EXCEPTION_DEBUG_EVENT | 发生异常时发送此类调试事件。 |
| EXIT_PROCESS_DEBUG_EVENT | 进程结束后发送此类调试事件。 |
| EXIT_THREAD_DEBUG_EVENT | 一个线程结束后发送此类调试事件。 |
| LOAD_DLL_DEBUG_EVENT | 装载一个DLL模块之后发送此类调试事件。 |
| OUTPUT_DEBUG_STRING_EVENT | 被调试进程调用OutputDebugString之类的函数时发送此类调试事件。 |
| RIP_EVENT | 发生系统调试错误时发送此类调试事件。 |
| UNLOAD_DLL_DEBUG_EVENT | 卸载一个DLL模块之后发送此类调试事件。 |

每种调试事件的详细信息通过联合体u来记录，通过u的字段的名称可以很快地判断哪个字段与哪种事件关联。

例如CREATE_PROCESS_DEBUG_EVENT调试事件的详细信息由CreateProcessInfo字段来记录。

dwProcessId和dwThreadId分别是触发调试事件的进程ID和线程ID。

一个调试器可能同时调试多个进程，而每个进程内又可能有多个线程，通过这两个字段就可以知道调试事件是从哪个进程的哪个线程触发的了。

调试器通过WaitForDebugEvent函数获取调试事件，通过ContinueDebugEvent继续被调试进程的执行。

ContinueDebugEvent有三个参数，第一和第二个参数分别是进程ID和线程ID，表示让指定进程内的指定线程继续执行。通常这是在一个循环中完成的

# ****读取寄存器和内存****

每个线程都有一个上下文环境，它包含了有关线程的大部分信息，例如线程栈的地址，线程当前正在执行的指令地址等。

上下文环境保存在寄存器中，系统进行线程调度的时候会发生上下文切换，实际上就是将一个线程的上下文环境保存到内存中，然后将另一个线程的上下文环境装入寄存器

获取某个线程的上下文环境需要使用GetThreadContext函数，该函数声明如下：

```c
BOOL WINAPI GetThreadContext(
	HANDLE hThread,
	LPCONTEXT lpContext
);
```

第一个参数是线程的句柄，第二个参数是指向CONTEXT结构的指针。

要注意，调用该函数之前需要设置CONTEXT结构的ContextFlags字段，

指明你想要获取哪部分寄存器的值。该字段的取值如下

ContextFlags.ContextFlags

[CONTEXT (winnt.h) - Win32 apps](https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-context)

该字段的取值如下

| CONTEXT_CONTROL | 获取EBP，EIP，CS，EFLAGS，ESP和SS寄存器的值。 |
| --- | --- |
| CONTEXT_INTEGER | 获取EAX，EBX，ECX，EDX，ESI和EDI寄存器的值。 |
| CONTEXT_SEGMENTS | 获取DS，ES，FS和GS寄存器的值。 |
| CONTEXT_FLOATING_POINT | 获取有关浮点数寄存器的值。 |
| CONTEXT_DEBUG_REGISTERS | 获取DR0，DR1，DR2，DR3，DR6，DR7寄存器的值。 |
| CONTEXT_FULL | 等于CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS |

调用GetThreadContext函数之后，CONTEXT结构相应的字段就会被赋值，此时就可以输出各个寄存器的值了

读取进程的内存使用ReadProcessMemory函数，该函数声明如下：

```c
BOOL WINAPI ReadProcessMemory(
    HANDLE hProcess,                  //进程句柄
    LPCVOID lpBaseAddress,            //要读取的地址
    LPVOID lpBuffer,                  //一个缓冲区的指针，保存读取到的内容
    SIZE_T nSize,                     //要读取的字节数
    SIZE_T* lpNumberOfBytesRead       //一个变量的指针，保存实际读取到的字节数
);
```

要想成功读取到进程的内存，需要两个条件：

一是hProcess句柄具有PROCESS_VM_READ的权限；

二是由lpBaseAddress和nSize指定的内存范围必须位于用户模式地址空间内，而且是已分配的

对于调试器来说，第一个条件很容易满足，因为调试器对被调试进程具有完整的权限，可以对其进行任意操作。

第二个条件意味着我们不能读取进程任意地址的内存，而是有一个限制。Windows将进程的虚拟地址空间分成了四个分区，如下表所示：

| 分区 | 地址范围 |
| --- | --- |
| 空指针赋值分区 | 0x00000000~0x0000FFFF |
| 用户模式分区 | 0x00010000~0x7FFEFFFF |
| 64KB禁入分区 | 0x7FFF0000~0x7FFFFFFF |
| 内核模式分区 | 0x80000000~0xFFFFFFFF |

空指针赋值分区主要为了帮助程序员检测对空指针的访问，任何对这一分区的读取或写入操作都会引发异常。

64KB禁入分区正如其名字所言，是禁止访问的，由Windows保留。

内核模式分区由Windows的内核部分使用，运行于用户态的进程不能访问这一区域。

进程只能访问用户模式分区的内存，对于其它分区的访问将会引发ACCESS_VIOLATION异常。

另外，并不是用户模式分区的任意部分都可以访问。

我们知道，在32位保护模式下，进程的4GB地址空间是虚拟的，在物理内存中不存在。

如果要使用某一部分地址空间的话，必须先向操作系统提交申请，让操作系统为这部分地址空间分配物理内存。

只有经过分配之后的地址空间才是可访问的，试图访问未分配的地址空间仍然会引发ACCESS_VIOLATION异常

# **调试符号**