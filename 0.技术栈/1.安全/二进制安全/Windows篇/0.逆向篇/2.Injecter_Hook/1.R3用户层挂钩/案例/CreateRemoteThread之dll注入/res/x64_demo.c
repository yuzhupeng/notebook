#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
DWORD is_x86 = 0;//进程架构
BOOL InjectDll(DWORD dwPID, char* szDllPath)
{
    HANDLE hProcess = NULL, hThread = NULL;
    HMODULE hMod = NULL;
    LPVOID pRemoteBuf = NULL;
    DWORD dwBufSize = strlen(szDllPath + 4);
    LPTHREAD_START_ROUTINE pThreadProc;


    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        printf("OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());//这里为完全控制权限，即所有访问权限（读、写、执行等）
        return FALSE;
    }

    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);//往目标进程写入数据

    if (is_x86)//下面这个短地址,在本进程是无法load或者get的
    {
        //hMod = LoadLibrary(TEXT("C:\\Windows\\SysWOW64\\kernel32.dll"));//因为一定会被
        //pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");
        pThreadProc = (LPTHREAD_START_ROUTINE)0x77531270;//LoadLibiaryA
        //77533C50
    }
    else
    {
        hMod = LoadLibrary("C:\\Windows\\System32\\kernel32.dll");//因为一定会被
        pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");
    }
    hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);//创建一个远程线程,指定了线程处理函数,线程参数(参数的地址是刚才在目标进程分配的)
    WaitForSingleObject(hThread, INFINITE);//等待远程线程执行完毕

    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}
/*
* 函数名: Get_Privilege
* 函数参数: 
* 参数1: TCAHR的权限名字
* 参数2: TRUE或者FALSE
* 函数功能: 提权
*/
BOOL Get_Privilege(TCHAR* lpszPrivilege, DWORD bEnablePrivilege)
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

    char* dllPath;
    DWORD dwPID;
    HANDLE hProcess;

    if (0)
    {
        if (argc != 3)
        {
            printf("USAGE : %s (pid) (dll_path)\n", argv[0]);//说明该如何使用该工具
            return 1;
        }
        dllPath = argv[2];
        dwPID = atoi(argv[1]);
    }
    else
    {
        dllPath = "f:/redqx32.dll";
        dwPID = FindProcessID(TEXT("calc.exe"));
        if (dwPID == -1)
        {
            return 1;
        }
    }
    if (!Get_Privilege(SE_DEBUG_NAME, TRUE))//提权
    {
        return 1;
    }
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
    IsWow64Process(hProcess, &is_x86);
    CloseHandle(hProcess);
    //让远程线程去加载动态我们的dll文件,到时就会自动执行那个dll的DllMain
    if (InjectDll(dwPID, dllPath))
    {
        printf("InjectDll(\"%s\") success!!!\n", dllPath);
    }
    else
    {
        printf("InjectDll(\"%s\") failed!!!\n", dllPath);
    }
    return 0;
}