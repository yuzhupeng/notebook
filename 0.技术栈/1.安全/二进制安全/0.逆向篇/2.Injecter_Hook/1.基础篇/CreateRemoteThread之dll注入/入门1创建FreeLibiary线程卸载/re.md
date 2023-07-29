# 入门1 创建FreeLibiary线程卸载

# 导入

之前,我们已经说过关于创建远程线程注入的相关东西

这里我么简单说一下创建远程线程卸载dll

其实和之前LoadLibiary差不多

只是多了一些进程的查找

因为期中要用到目标进程的pid,pid的获取需要遍历一些东西

头文件

```c
#include "windows.h"
#include "tlhelp32.h"
#include "tchar.h"

#define DEF_PROC_NAME	(L"snipaste.exe")
#define DEF_DLL_NAME	(L"Dll1.dll")
```

主函数如下(管理员下运行)

```c
int _tmain(int argc, TCHAR* argv[])
{
    DWORD dwPID = 0xFFFFFFFF;

    // find process
    dwPID = FindProcessID(DEF_PROC_NAME);
    if (dwPID == 0xFFFFFFFF)
    {
        _tprintf(L"There is no <%s> process!\n", DEF_PROC_NAME);
        return 1;
    }

    _tprintf(L"PID of \"%s\" is %d\n", DEF_PROC_NAME, dwPID);

    // change privilege
    if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
        return 1;

    // eject dll
    if (EjectDll(dwPID, DEF_DLL_NAME))
        _tprintf(L"EjectDll(%d, \"%s\") success!!!\n", dwPID, DEF_DLL_NAME);
    else
        _tprintf(L"EjectDll(%d, \"%s\") failed!!!\n", dwPID, DEF_DLL_NAME);

    return 0;
}
```

先获取目标进程的pid

然后提权

最后是卸载dll

# 获取目标进程PID

```c
#include <tlhelp32.h>
DWORD FindProcessID(TCHAR* szProcessName)//
{
    DWORD dwPID = 0xFFFFFFFF;
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;

    // Get the snapshot of the system
    pe.dwSize = sizeof(PROCESSENTRY32);
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

    // find process
    Process32First(hSnapShot, &pe);
    do
    {
#ifdef UNICODE
        if (!wcscmp(szProcessName, pe.szExeFile))
        
#else
        if (!strcmp(szProcessName, pe.szExeFile))
        
#endif
        {
            dwPID = pe.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapShot, &pe));

    CloseHandle(hSnapShot);

    return dwPID;
}
```

其实没有太多的东西去深究,目前也没太大的意义

知道通过这么写可有获取目标进程PID就可以了

# 卸载dll的代码

```c
BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName)
{
    BOOL bMore = FALSE, bFound = FALSE;
    HANDLE hSnapshot, hProcess, hThread;
    HMODULE hModule = NULL;
    MODULEENTRY32 me = { sizeof(me) };
    LPTHREAD_START_ROUTINE pThreadProc;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

    bMore = Module32First(hSnapshot, &me);
    for (; bMore; bMore = Module32Next(hSnapshot, &me))
    {
        if (!_tcsicmp((LPCTSTR)me.szModule, szDllName) ||!_tcsicmp((LPCTSTR)me.szExePath, szDllName))
        {
            bFound = TRUE;
            break;
        }
    }

    if (!bFound)
    {
        CloseHandle(hSnapshot);
        return FALSE;
    }

    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        _tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
        return FALSE;
    }

    hModule = GetModuleHandle(L"kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
    hThread = CreateRemoteThread(hProcess, NULL, 0,pThreadProc, me.modBaseAddr, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hProcess);
    CloseHandle(hSnapshot);

    return TRUE;
}
```

也没有太多的东西去说

其中可能会疑惑的是

```c
    for (; bMore; bMore = Module32Next(hSnapshot, &me))
    {
        if (!_tcsicmp((LPCTSTR)me.szModule, szDllName) ||!_tcsicmp((LPCTSTR)me.szExePath, szDllName))
        {
            bFound = TRUE;
            break;
        }
    }
```

me.szModul和me.szExePath分别代表了模块名字和模块的完整路径

# 关于其它卸载问题

书上所: pe文件直接导入的dll文件是无法在进程运行过程中直接卸载

原因如下：

**模块引用计数不为零**：

当一个 DLL 文件被一个或多个 PE 文件加载后，它的“模块引用计数”会增加，

只有当这个计数归零时，才能将其卸载。

如果一个 PE 文件中直接导入了 DLL 文件，并且该 DLL 文件中的任何一个函数正在被该 PE 文件使用，那么该 DLL 文件的模块引用计数就不为零，

因此无法被卸载。

**资源泄漏**：PE 文件中直接导入的 DLL 文件可能会在运行期间分配一些资源（如内存、句柄等），

如果在释放这些资源时出现问题，可能会导致 DLL 文件无法被卸载。

**依赖关系**：PE 文件中直接导入的 DLL 文件可能又依赖了其他 DLL 文件，这些文件也必须被卸载才能彻底卸载当前 DLL 文件。

如果存在循环依赖关系，就可能导致 DLL 文件无法被卸载。

因此，如果想要在 PE 文件中直接导入 DLL 文件并能够在运行期间进行卸载，

需要确保 DLL 文件的模块引用计数为零、资源已经被妥善释放、以及它所依赖的其他 DLL 文件都已经被卸载。

通常情况下，为了避免出现这些问题，建议使用动态链接方式来引用 DLL 文件。