# 记事本-NtResumeThread() 之 进程隐藏

[mycode/Re/API_HOOK/记事本-NtQuerySystemInformation_进程隐藏/全局钩子-NtResumeThread at 53aeb0d0ae0d86f03fc8393575912ded00dfafeb · redqx/mycode](https://github.com/redqx/mycode/tree/53aeb0d0ae0d86f03fc8393575912ded00dfafeb/Re/API_HOOK/记事本-NtQuerySystemInformation_进程隐藏/全局钩子-NtResumeThread)

之前那个进程隐藏用的是createProcess和NtQuerySystemInformation() 钩取explorer.exe

也提到过全局API钩取的概念,也提到过NtResumeThread()

这一次就简单实现一下,他的原理和之前是一样的,只不过它更加底层罢了

但是,也还是简答的说一下他的相关实现

其实,也没干什么

说一下dllmain函数

```jsx
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {

    case DLL_PROCESS_ATTACH:
        hook_by_code(szNtdll, szNtQuerySystemInformatio, g_OldAPI_1,my_NtQuerySystemInformation);
        hook_by_code(szNtdll, szNtResumeThread, g_OldAPI_2, my_NtResumeThread);

        break;
    case DLL_PROCESS_DETACH:
        unhook_by_code(szNtdll, szNtQuerySystemInformatio, g_OldAPI_1);
        unhook_by_code(szNtdll, szNtResumeThread, g_OldAPI_2);
  
        break;
    }
    return TRUE;
}
```

多了一个hook和unhook

具体的函数,看上去比较复杂

但是也还是没啥东西的

```c
NTSTATUS WINAPI my_NtResumeThread(HANDLE ThreadHandle, PULONG SuspendCount)
{
    NTSTATUS status = 0, statusThread;
    FARPROC pFunc = NULL, pFuncThread = NULL;
    DWORD dwPID = 0;
    static DWORD dwPrevPID = 0;
    THREAD_BASIC_INFORMATION tbi;
    HMODULE hMod = NULL;
    CHAR szModPath[MAX_PATH] = { 0, };
    HANDLE hProcess;

    hMod = GetModuleHandleA(szNtdll);
    if (hMod == NULL)
    {
        DebugLog("NewNtResumeThread() : GetModuleHandle() failed!!! [%d]\n", GetLastError());
        return NULL;
    }

    // call ntdll!ZwQueryInformationThread()
    pFuncThread = GetProcAddress(hMod, "ZwQueryInformationThread");//注意这个函数是Nt版本,同时他的名字是ZwQueryInformationThread,不是NtQuerySystemInformation
    DebugLog("Zw %p\n", pFuncThread);
    if (pFuncThread == NULL)
    {
        DebugLog("NewNtResumeThread() : GetProcAddress() failed!!! [%d]\n", GetLastError());
        return NULL;
    }

    statusThread = ((PFZWQUERYINFORMATIONTHREAD)pFuncThread)(ThreadHandle, 0, &tbi, sizeof(tbi), NULL);
    if (statusThread != 0)
    {
        DebugLog("NewNtResumeThread() : pFuncThread() failed!!! [%d]\n", GetLastError());
        return NULL;
    }

    dwPID = (DWORD)tbi.ClientId.UniqueProcess;//获取主线程dwPID

    //打开进程病注入
    if ((dwPID != GetCurrentProcessId()) && (dwPID != dwPrevPID))
    {
        DebugLog("NewNtResumeThread() => call InjectDll()\n");

        dwPrevPID = dwPID;

        // change privilege 如果权限已经够了,就不需要提权了
        //if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
        //DebugLog("NewNtResumeThread() : SetPrivilege() failed!!!\n");

        // get injection dll path
        GetModuleFileNameA(GetModuleHandleA(STR_MODULE_NAME), szModPath, MAX_PATH);
        if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
        {
            DebugLog("OpenProcess failed");
            return FALSE;
        }
        if (!InjectDll_hook(hProcess, szModPath))
        {
            DebugLog("NewNtResumeThread() : InjectDll(%d) failed!!!\n", dwPID);
        }
    }

    // call ntdll!NtResumeThread() 脱钩
    if (!unhook_by_code(szNtdll, szNtResumeThread, g_OldAPI_2))
    {
        DebugLog("NewNtResumeThread() : unhook_by_code() failed!!!\n");
        return NULL;
    }

    pFunc = GetProcAddress(hMod, "NtResumeThread");
    if (pFunc == NULL)
    {
        DebugLog("NewNtResumeThread() : GetProcAddress() failed!!! [%d]\n",
            GetLastError());
        goto __NTRESUMETHREAD_END;
    }

    status = ((PFNtResumeThread)pFunc)(ThreadHandle, SuspendCount);//正常调用
    if (status != 0)
    {
        DebugLog("NewNtResumeThread() : pFunc() failed!!! [%d]\n", GetLastError());
        goto __NTRESUMETHREAD_END;
    }

__NTRESUMETHREAD_END:

    if (!hook_by_code(szNtdll, szNtResumeThread, g_OldAPI_2, (PROC)my_NtResumeThread))//挂钩
    {
        DebugLog("NewNtResumeThread() : hook_by_code() failed!!!\n");
    }

    DebugLog("NewNtResumeThread() : end!!!\n");

    return status;
}
```

首先是根据 `NTSTATUS WINAPI NtResumeThread(HANDLE ThreadHandle, PULONG SuspendCount)` 

的`HANDLE ThreadHandle` 来获取主线程所在进程的信息,然后对它注入

然后对 `NtResumeThread` 脱钩,然后调用它,然后挂钩

其中有个小小的疑惑:

他是如何成功调用 "ZwQueryInformationThread"的

于是问了chatgpt

事实上，**`NtQueryInformationThread`** 和 **`ZwQueryInformationThread`** 两者都是系统调用函数，并且它们都是可以在用户层调用的。这两个函数都用于在用户空间和内核空间之间进行通信，以获取线程信息。

**`NtQueryInformationThread`** 和 **`ZwQueryInformationThread`** 函数的区别在于它们的调用方式和参数传递方式，具体如下：

1. **调用方式**:
    - **`NtQueryInformationThread`**: 是用于调用内核模式下的系统调用的函数。在用户层，可以通过在 **`ntdll.dll`** 动态链接库中找到 **`NtQueryInformationThread`** 函数的地址，并通过函数指针或其他技术来调用它。由于这个函数并不是官方公开的API，因此调用它可能不稳定且存在潜在的兼容性问题。
    - **`ZwQueryInformationThread`**: 也是用于调用内核模式下的系统调用的函数。在用户层，可以通过调用 **`ntdll.dll`** 中的 **`ZwQueryInformationThread`** 函数来触发系统调用。这是官方提供的、稳定的系统调用接口。
2. **参数传递**:
    - **`NtQueryInformationThread`**: 参数传递是通过将参数放入通用寄存器中进行的，如 **`RAX`**, **`RCX`**, 等，然后通过软中断（例如 **`int 0x2E`**）来触发系统调用。
    - **`ZwQueryInformationThread`**: 参数传递是通过函数的传统方式，即在用户模式下使用栈来传递参数，并使用 **`syscall`** 指令来触发系统调用。

综上所述，你可以在用户层调用 **`ZwQueryInformationThread`** 函数，它是一个稳定且受支持的方法。虽然你也可以在用户层调用 **`NtQueryInformationThread`**，但不推荐这样做，因为它是一个未公开的API，可能在不同的Windows版本中发生变化，并且存在潜在的稳定性和兼容性问题