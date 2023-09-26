# 全局的hook x64分析

[mycode/Re/API_HOOK/notepad_进程隐藏/全局钩子 at 289009d3d4f73f60111e7285c7c6e892e6f58386 · redqx/mycode](https://github.com/redqx/mycode/tree/289009d3d4f73f60111e7285c7c6e892e6f58386/Re/API_HOOK/notepad_进程隐藏/全局钩子)

之所以全局就是先于某些东西比如

进程的创建都要通过CreateProcessA胡总CreateProcessW

进程创建都是父进程完成的,父进程通常是explore.exe

所以我们可以钩取explore.exe的CreateProcessA和CreateProcessW来实现进程隐藏

CreateProcessA()、CreateProcessW()函数内部又分别调用了CreateProcessInternalA()、CreateProcessInternelW()函数

若可能，尽量钩取低级API

但是还有比CreateProcessA胡总CreateProcessW更加低级的API可以钩取

钩取函数（NewCreateProcess）要钩取调用原函数（CreateProcess）而创建的子进程的API。
因此，极短时间内，子进程可能在未钩取的状态下运行。

我们进行全局API钩取时必须解决上面这些问题。

幸运的是，很多代码逆向分析高手通过努力发现了比kernel32.CreateProcess()更低级的API,

钩取它效果会更好（能够一次性解决上面所有问题）。

这个API就是ntdll.ZwResumeThread() API

```c
ZwResumeThread(
    IN	HANDLE	ThreadHandle,
    OUT	PULONG	SuspendCount OPTIONAL
);
```

> 用户模式下，NtXXX系列与ZwXXX系列仅是名称不同，它们其实是相同的API。
> 

ZwResumeThread()函数（出处：MSDN）在进程创建后、主线程运行前被调用执行（在CreateProcess()API内部调用执行）。

所以只要钩取这个函数，即可在不运行子进程代码的状态下钩取API。

但需要注意的是，ZwResumeThread()是一个尚未公开的API，将来的某个时候可能会被改变，这就无法保障安全性。

所以，钩取类似ZwResumeThread()的尚未公开API时，要时刻记得，随着OS补丁升级，该API可能更改，

这可能使在低版本中正常运行的钩取操作到了新版本中突然无法正常运行

所以简单的说一下，书上的那个例子

书上的例子是对explorer.exe进行的一个hook

也就每次进程的创建都会被我们注入一个dll

ps： 只是针对于x64的进程的x64的dll

每次新的进程创建，先注入我们的dll，然后再创建一个进程

所以dllMian就是

```jsx
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {

    case DLL_PROCESS_ATTACH:
        hook_by_code(szNtdll, szNtQuerySystemInformatio, g_OldAPI_1,my_NtQuerySystemInformation);
        hook_by_code(szKernel32, szCreateProcessA, g_OldAPI_2,NewCreateProcessA);
        hook_by_code(szKernel32, szCreateProcessW, g_OldAPI_3,NewCreateProcessW);
        break;
    case DLL_PROCESS_DETACH:
        unhook_by_code(szNtdll, szNtQuerySystemInformatio, g_OldAPI_1);
        unhook_by_code(szKernel32, szCreateProcessA, g_OldAPI_2);
        unhook_by_code(szKernel32, szCreateProcessW, g_OldAPI_3);
        break;
    }
    return TRUE;
}
```

其他hook的流程，都和我们之前的

hook_by_code(szNtdll, szNtQuerySystemInformatio, g_OldAPI_1,my_NtQuerySystemInformation);

unhook_by_code(szNtdll, szNtQuerySystemInformatio, g_OldAPI_1);

是类似的做法

然后就大概体积一下额外Hook的东西

```jsx
BOOL WINAPI NewCreateProcessA(
    LPCTSTR lpApplicationName,
    LPTSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCTSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
)
{
    BOOL bRet;
    FARPROC pFunc;
    int is_x86=0;
    // unhook
    unhook_by_code(szKernel32,szCreateProcessA, g_OldAPI_2);

    // original API 호출
    pFunc = GetProcAddress(GetModuleHandleA(szKernel32),szCreateProcessA);
    bRet = ((PFCREATEPROCESSA)pFunc)(lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation);

    //不一定要hook
    IsWow64Process(lpProcessInformation->hProcess, &is_x86);
    if (bRet&&(!is_x86))
    {
        InjectDll_hook(lpProcessInformation->hProcess, STR_MODULE_NAME);
    }
    hook_by_code(szKernel32,szCreateProcessA, g_OldAPI_2, NewCreateProcessA);

    return bRet;
}

BOOL WINAPI NewCreateProcessW(
    LPCTSTR lpApplicationName,
    LPTSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCTSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
)
{
    BOOL bRet;
    FARPROC pFunc;
    int is_x86=0;

    unhook_by_code(szKernel32, szCreateProcessW, g_OldAPI_3);
    pFunc = GetProcAddress(GetModuleHandleA(szKernel32), szCreateProcessW);
    bRet = ((PFCREATEPROCESSW)pFunc)(lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation);

    //不一定要hook
    IsWow64Process(lpProcessInformation->hProcess, &is_x86);

    if (bRet&&(!is_x86))
    {
        InjectDll_hook(lpProcessInformation->hProcess, STR_MODULE_NAME);
    }
    // hook
    hook_by_code(szKernel32, szCreateProcessW, g_OldAPI_3, NewCreateProcessW);

    return bRet;
}
```

可以看出,其实也没干什么

和之前的东西差不多

所以以后，单凡开一个进程，都会被注入我们的东西