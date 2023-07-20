# 入门1 创建LoadLibiary线程注入

遗留的问题:

为什么只有开管理员权限才可以注入成功呢??

# 注入器源码分析

首先看一下main函数

```c
int main(int argc,char* argv[]) 
{
    if (argc != 3)
    {
        printf("USAGE : %s (pid) (dll_path)\n", argv[0]);//说明该如何使用该工具
        return 1;
    }

    if (!Get_Privilege(SE_DEBUG_NAME, TRUE))//提权
    {
        return 1;
    } 

    //让远程线程去加载动态我们的dll文件,到时就会自动执行那个dll的DllMain
    if (InjectDll((DWORD)atoi(argv[1]), argv[2]))
    {
        printf("InjectDll(\"%s\") success!!!\n", argv[2]);
    }
    else
    {
        printf("InjectDll(\"%s\") failed!!!\n", argv[2]);
    }
    return 0;
}
```

他的流程就是

先提权然后去注入

## 提权三件套

在提权过程中，通常需要使用以下三个函数来完成：

1. **`OpenProcessToken`**: 这个函数用于打开进程的访问令牌（Access Token）。
    
    访问令牌是一种用于标识和验证进程身份和权限的数据结构。**通过打开进程的访问令牌，可以获取到对该进程进行权限修改的权限。**
    
2. **`LookupPrivilegeValue`**: 这个函数用于查找指定特权（Privilege）的特权标识符（LUID）。特权标识符是一种**唯一的数值，用于表示系统中的不同特权**。在提权过程中，需要使用特权标识符来标识要修改的特权。
3. **`AdjustTokenPrivileges`**: 这个函数用于调整访问令牌（Access Token）中的特权。**通过提供特权标识符和特权的状态信息，可以将指定的特权授予或撤销访问令牌**。在提权过程中，需要使用该函数来修改访问令牌中的特权，以获取所需的权限。

下面是它们的配合过程：

1. 首先，使用 **`OpenProcessToken`** 打开目标进程的访问令牌，以获取对其进行修改的权限。
2. 然后，使用 **`LookupPrivilegeValue`** 查找指定特权的特权标识符（LUID），通常是 **`SE_DEBUG_NAME`** 特权。
3. 接下来，创建一个 **`TOKEN_PRIVILEGES`** 结构，将要修改的特权标识符和特权的状态信息填充进去。
4. 最后，使用 **`AdjustTokenPrivileges`** 函数将特权授权或撤销给访问令牌。函数返回成功表示特权调整成功。

这样，通过配合使用这三个函数，可以在提权过程中打开目标进程的访问令牌，查找特权标识符，并将特权授权给访问令牌，以获取所需的权限。

我的小结:

获取令牌的Token : **`OpenProcessToken`** 

查找特权的uuid : **`LookupPrivilegeValue`**   

开一个新的权限结构体 `TOKEN_PRIVILEGES` ,然后根据已有的或者想要的给它初始化

然后把新的权限结构体注入到指定进程的令牌中 **`AdjustTokenPrivileges`**

关于访问令牌的一些介绍:

访问令牌（access token）是一个数据结构，用于在 Windows 操作系统中标识和验证身份认证。

每个进程都有一个访问令牌，该令牌用于标识该进程所属的用户或用户组，并规定了该进程可以执行的操作。

某些操作需要特殊的权限才能执行，这些权限被授予给某些用户或用户组，

当一个进程需要执行这些需要特殊权限的操作时,就需要使用访问令牌来检查其是否具有足够的权限。

因此，访问令牌在 Windows 操作系统中扮演着至关重要的角色，能够保障系统和应用程序的安全性和稳定性。

- 提权代码
    
    用到的api
    
    OpenProcessToken(打开当前的),LookupPrivilegeValue(寻找指定的),AdjustTokenPrivileges(设置当前的)
    
    ```c
    BOOL Get_Privilege(char* lpszPrivilege, DWORD bEnablePrivilege)
    {
        TOKEN_PRIVILEGES tp;
        HANDLE hToken;
        LUID luid;
    
        if (!OpenProcessToken(
            GetCurrentProcess(),//获取当前进程的句柄
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, // 要修改权限+需要查询信息
            &hToken)//即函数调用成功后将获得的令牌句柄存储到这个变量中。
            )
        {
            printf("OpenProcessToken error: %u\n", GetLastError());
            return FALSE;
        }
    
        if (!LookupPrivilegeValue(//该函数用于检索给定特权名称的特权标识符
                NULL,//NULL表示将使用本地系统来执行特权查找操作
                lpszPrivilege, //字符串,指定要查找其特权标识符的特权名称。
                &luid//输出变量,这是一个指向LUID（特权标识符）变量的指针
            )
        )    
        {
            printf("LookupPrivilegeValue error: %u\n", GetLastError());
            return FALSE;
        }
    
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        if (bEnablePrivilege)
        {
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        }
        else
        {
            tp.Privileges[0].Attributes = 0;
        }
    
        if (!AdjustTokenPrivileges(
                hToken, //从OpenProcessToken获取的
                FALSE,
                &tp,//从LookupPrivilegeValue获取的
                sizeof(TOKEN_PRIVILEGES),
                (PTOKEN_PRIVILEGES)NULL,
                (PDWORD)NULL
                )
            )
        {
            printf("AdjustTokenPrivileges error: %u\n", GetLastError());
            return FALSE;
        }
    
        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
        {
            printf("The token does not have the specified privilege. \n");
            return FALSE;
        }
    
        return TRUE;
    }
    ```
    

## 注入分析

```c
BOOL InjectDll(DWORD dwPID, char* szDllPath)
{
    HANDLE hProcess = NULL, hThread = NULL;
    HMODULE hMod = NULL;
    LPVOID pRemoteBuf = NULL;
    DWORD dwBufSize = strlen (szDllPath+4);
    LPTHREAD_START_ROUTINE pThreadProc;

  
    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        printf("OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());//这里为完全控制权限，即所有访问权限（读、写、执行等）
        return FALSE;
    }

    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);//往目标进程写入数据
    hMod = GetModuleHandleA("kernel32.dll");//因为一定会被
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");

    hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);//创建一个远程线程,指定了线程处理函数,线程参数(参数的地址是刚才在目标进程分配的)
    WaitForSingleObject(hThread, INFINITE);//等待远程线程执行完毕

    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}
```

执行流程就是

打开目标进程

在目标进程开辟内存

把我们的dll路径写入该进程

然后创建远程线程,,线程处理函数是`LoadLibraryA`, 线程参数是 目标进程中刚刚写入的dll的地址

然后等待远程线程执行介绍

关闭所有句柄

ps:`LoadLibraryA` 函数是kernel32的,所以在当前PC的所有进程中,函数地址是一样的

# DLL分析

```c
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    HANDLE hThread = NULL;
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugString("Success");
        hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
        if (hThread)
        {
            CloseHandle(hThread);
        }
        break;
    }

    return TRUE;
}
```

一个dll被动态的加载,一定会执行dllmain的

于是我们在dllmain里面写入一些东西,就会在加载dll的时候执行

比如我们这里干的事情就是dbg输出,然后开启一个线程,然后关闭线程

线程处理函数看你想干什么,我们这里也没干什么

也就是弹个窗口

```c

DWORD WINAPI ThreadProc(LPVOID lParam)
{
    MessageBoxA(0, "你好", "dqx", 0);
    return 0;
}
```

所以效果如下(只有给他管理员权限才可以)

![Untitled](%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6/Untitled.png)

# 关于注入的IDA调试

被注入的目标,为了给注入增加时间,我们就sleep了60s

```c
#include <windows.h>
#include <stdio.h>

void main()
{
	DWORD dwPid = GetCurrentProcessId();
	printf("%d", dwPid);
	Sleep(60 * 1000);
	return; //也就睡半分钟
}
```

注入器的源码在之前我已经写了

首先我们对IDA的调试做一个设置,在加载一个新的dll的时候就停止下来

![Untitled](%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6/Untitled%201.png)

至于停在了哪里,我也不知道..实际上是每次都停在了ntdll的某个地址,比如下面这个东东

![Untitled](%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6/Untitled%202.png)

首先运行 target.exe (也就是上面那个源码)

一直按下F9 因为开始的时候会加载很多dll,然那些和我们关系不太大的dll跳过

当我们程序真正的执行了,差不多下面这个样子

寄存器无数据,EIP也没有数据

![Untitled](%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6/Untitled%203.png)

于是开始我们的注入

![Untitled](%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6/Untitled%204.png)

然后IDA那边就停下来了

不用多说,它停在了ntdll,不知道为什么

```c
769A1270: thread has started (tid=22500) 
77D40000: loaded E:\Code\normal\C\2022\Often\Debug\Dll1.DLL
PDBSRC: loading symbols for 'C:\Windows\SysWOW64\ntdll.dll'...
```

观察IDA加载的模块,可以发现我们注入的dll已经进入了

![Untitled](%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6/Untitled%205.png)

然后的话,,,我不太想通过字符串等一些其它信息搜索我们那个dll的关键函数或者dllmain函数

而是通过dll的相关pe信息去完成

一开始,IDA还不会去加载我们才载入的dll1.dll的相关信息

我们需要手动去载入一些信息

![Untitled](%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6/Untitled%206.png)

然后我们就可以看到dll1.dll的符号信息了

![Untitled](%E5%85%A5%E9%97%A81%20%E5%88%9B%E5%BB%BALoadLibiary%E7%BA%BF%E7%A8%8B%E6%B3%A8%E5%85%A5%20ceda0f3bcc834c1aa72b31a4711205a6/Untitled%207.png)

然后就可以愉快的获取我们想要的信息了

后面要在dll1.dll下断点什么的都很容易了

如果加载符号表失败,但是我们想要进入DllMain函数怎么办?

一般情况下,Dll的ep去往的就是DLlmain

所以我们可以写一个IDC去往Dll的ep

但是懒得写了

# 记录一次关于注入-键盘记录器

## 关于对为什么记录失败的疑惑

首先展示一下target源码,只是展示,未深入了解

```c
#include <windows.h>
#include <stdio.h>

int main()
{
	MSG msg;
	DWORD dwPid = GetCurrentProcessId();
	printf("%d", dwPid);
	while (GetMessage(&msg, NULL, 0, 0))
	{

	}
	return; //也就睡半分钟
}
```

然后我们的注入器,也没干什么

其中他的循环用的是 

```c

while (GetMessage(&msg, NULL, 0, 0))
{

}
```

而不是使用一些类似于 `while(1)` 或者`Sleep(60*1000)`  

我说你不这么用呢?

后来直接就g了

码中的 **`while (GetMessage(&msg, NULL, 0, 0)) {}`** 是一个消息循环，

它会不断地获取消息并处理，包括键盘输入等事件。

当没有消息时，**`GetMessage`** 函数会等待新的消息到达

 **`Sleep`** 函数只是简单地使程序进入睡眠状态，不会处理消息。

 **`while (1)`**，会导致程序进入一个无限循环而没有处理消息的机会，

从而导致键盘记录无法正常工作。程序会持续进行空循环，不会处理消息，

而是一直在快速地检查条件。这种快速的循环称为"忙等待"，会导致CPU资源过度占用，使得程序看起来卡顿。

首先对于目标进程…肯定需要一个类似于循环的东西维持程序不推出

不退出有3种

1), while(1)

2), Sleep()

3), GetMessage()

| Dll线程使用情况 | 主线程 | 效果 |
| --- | --- | --- |
| while(1) | while (GetMessage(&msg, NULL, 0, 0)) | 无效果&很卡 |
| while(1) | while(1) | 无效果&很卡 |
| while(1) | Sleep() | 无效果&很卡 |
| Sleep() | while (GetMessage(&msg, NULL, 0, 0)) | 无效果&很卡 |
| Sleep() | while(1) | 无效果&很卡 |
| Sleep() | Sleep() | 无效果&很卡 |
| while (GetMessage(&msg, NULL, 0, 0)) | Sleep() | 正常记录 |
| while (GetMessage(&msg, NULL, 0, 0)) | while(1) | 正常记录 |
| while (GetMessage(&msg, NULL, 0, 0)) | while (GetMessage(&msg, NULL, 0, 0)) | 正常记录 |
| 无 | Sleep() | 无效果 |
| 无 | while(1) | 无效果 |
| 无 | while (GetMessage(&msg, NULL, 0, 0)) | 无效果 |

所以这是为什么?

## 为什么注入失败的的思考

首先回到注入器的源码:

先不谈提权是否成功

它后面有一段代码是什么呢?

就算等待创建的远程线程指向成功后,线程返回.注入器才算成功注入

只要原创线程不返回就不会显示注入成功

```c
    hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
		//创建一个远程线程,指定了线程处理函数,线程参数(参数的地址是刚才在目标进程分配的)
    WaitForSingleObject(hThread, INFINITE);//等待远程线程执行完毕

    CloseHandle(hThread);
    CloseHandle(hProcess);
```

然后我们思考一下,他会怎么返回?如何返回

当我们加载一个dll,进入Dllmain

然后通过那个`case DLL_PROCESS_ATTACH:` 算是一次进入,然后Dllmain函数也会return, 这算一次返回

当一个进程结束的时候, `case DLL_PROCESS_DETACH:`  算是一次进入,然后Dllmain函数也会return, 也算返回

如果我们在运行远程线程 LoadLibiary(”my_dll_path”)的时候,如果指定的dll加载的时候,dllmain不返回

那么LoadLibiary也不会返回,于是远程线程就那么一直不返回

于是我们的注入器就会一直等待,就会导致注入失败

## LoadLibiary和DllMain的联系

Libiary是否成功?

成功返回dll的基地址

失败返回0

但是dll一直不返回呢?

这就和我今天看到的Dllmain函数返回有关系了

如果我们的Dllmain一直不返回的话,其实我们的`LoadLibraryA` 一直不返回,相对于LoadLibraryA卡死了

## 挂钩失败?

注入失败归注入失败

但是我们在`case DLL_PROCESS_ATTACH` 和Dllmain函数结束这个时间段中,,,我们已经设置了全局的键盘钩子了呀…

难道我们也挂钩失败了?

可以延伸一些其它我目前还无法解集的问题

GetMessage(&msg, NULL, 0, 0) 的原理是从当前线程的消息队列中获取消息

我么在Dll中维护了一个线程,在main函数中又维护了一个主线程

问题来了…

假如我们在Dll线程和主线程都设置了一个消息循环

那么消息会进入哪个线程的消息队列

消息会给谁处理

在之前,

# 该代码的不足

提权失败,算注入失败

打开远程进程失败,算注入失败

于是其它失败或者成功都会显示 注入成功

所以的话,如果我写入一个乱七八糟的dll路径的话,那么也算是注入成功

于是困惑了我很久,g