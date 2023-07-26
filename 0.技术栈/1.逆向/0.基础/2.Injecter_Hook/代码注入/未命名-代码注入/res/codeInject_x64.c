#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

typedef struct _THREAD_PARAM
{
    FARPROC pFunc[2];               // LoadLibraryA(), GetProcAddress()
    char    szBuf[4][128];          // "user32.dll", "MessageBoxA", "www.reversecore.com", "ReverseCore"
} THREAD_PARAM, * PTHREAD_PARAM;

typedef HMODULE(WINAPI* PFLOADLIBRARYA)
(
    LPCSTR lpLibFileName
    );

typedef FARPROC(WINAPI* PFGETPROCADDRESS)
(
    HMODULE hModule,
    LPCSTR lpProcName
    );

typedef int (WINAPI* PFMESSAGEBOXA)
(
    HWND hWnd,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT uType
    );

BYTE* getReallCall(BYTE* lp)
{
   return  *(DWORD*)(lp + 1) + (BYTE*)lp + 5;
}
DWORD WINAPI ThreadProc(LPVOID lParam)
{
    PTHREAD_PARAM   pParam = (PTHREAD_PARAM)lParam;
    HMODULE         hMod = NULL;
    FARPROC         pFunc = NULL;

    // LoadLibrary()
    hMod = ((PFLOADLIBRARYA)pParam->pFunc[0])(pParam->szBuf[0]);    // "user32.dll"
    if (!hMod)
        return 1;

    // GetProcAddress()
    pFunc = (FARPROC)((PFGETPROCADDRESS)pParam->pFunc[1])(hMod, pParam->szBuf[1]);  // "MessageBoxA"
    if (!pFunc)
        return 1;

    // MessageBoxA()
    ((PFMESSAGEBOXA)pFunc)(NULL, pParam->szBuf[2], pParam->szBuf[3], MB_OK);

    return 0;
}

BOOL InjectCode(DWORD dwPID)
{
    HMODULE         hMod = NULL;
    THREAD_PARAM    param = { 0, };
    HANDLE          hProcess = NULL;
    HANDLE          hThread = NULL;
    LPVOID          pRemoteBuf[2] = { 0, };
    DWORD64         dwSize = 0,fs,fe;
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
        (LPVOID)fs,   // lpBuffer
        dwSize,                             // nSize
        NULL))                              // [out] lpNumberOfBytesWritten
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

//Get_Privilege(SE_DEBUG_NAME, TRUE)
BOOL Get_Privilege(TCHAR* lpszPrivilege, DWORD bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if (!OpenProcessToken( GetCurrentProcess(),//获取当前进程的句柄
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
int main(int argc, char* argv[])
{
    DWORD dwPID = 0;
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
    // change privilege
    if (dwPID&& !Get_Privilege(SE_DEBUG_NAME, TRUE))
    {
        return 1;
    }
    InjectCode(dwPID);
    return 0;
}
