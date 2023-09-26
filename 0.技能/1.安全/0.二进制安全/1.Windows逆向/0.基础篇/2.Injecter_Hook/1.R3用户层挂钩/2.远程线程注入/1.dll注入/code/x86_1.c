#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<stdio.h>
#include<string.h>
#include <tlhelp32.h>
#define my_test 1

void log(const char* format, ...)
{
    va_list vl;
    char szLog[512] = { 0, };

    va_start(vl, format);
    sprintf(szLog, format, vl);
    va_end(vl);

    OutputDebugStringA(szLog);
}

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
        log("OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }

    if (!LookupPrivilegeValueA(//该函数用于检索给定特权名称的特权标识符
        NULL,//NULL表示将使用本地系统来执行特权查找操作
        lpszPrivilege, //字符串,指定要查找其特权标识符的特权名称。
        &luid//输出变量,这是一个指向LUID（特权标识符）变量的指针
    )
        )
    {
        log("LookupPrivilegeValue error: %u\n", GetLastError());
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

        log("AdjustTokenPrivileges error: %u\n", GetLastError());
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        log("The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}
BOOL inject_Dll(DWORD dwPID, char* szDllPath)
{
    HANDLE hProcess = NULL, hThread = NULL;
    HMODULE hMod = NULL;
    LPVOID pRemoteBuf = NULL;
    DWORD dwBufSize = strlen(szDllPath + 4);
    LPTHREAD_START_ROUTINE pThreadProc;


    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        log("OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());//这里为完全控制权限，即所有访问权限（读、写、执行等）
        return FALSE;
    }

    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    if (pRemoteBuf)
    {
        WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);//往目标进程写入数据
        hMod = GetModuleHandleA("kernel32.dll");
        //这里加载的虽然是自己的kernel32.dll地址
        //但是地址是通用的,如果你是x86那么就加载x86的地址,如果你是x64就加载x64的地址
        if (hMod) {
            pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");
            hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);//创建一个远程线程,指定了线程处理函数,线程参数(参数的地址是刚才在目标进程分配的)
            if (hThread)
            {
                WaitForSingleObject(hThread, INFINITE);//等待远程线程执行完毕
                CloseHandle(hThread);
            }

        }
        VirtualFreeEx(hProcess, pRemoteBuf, dwBufSize, MEM_FREE);
    }
    CloseHandle(hProcess);
    return TRUE;
}
BOOL deject_Dll(DWORD dwPID, char* szDllName)
{
    BOOL bMore = FALSE, bFound = FALSE;
    HANDLE hSnapshot, hProcess, hThread;
    HMODULE hModule = NULL;
    MODULEENTRY32 me = { sizeof(me) };
    LPTHREAD_START_ROUTINE pThreadProc;
    DWORD result = 0;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    bMore = Module32First(hSnapshot, &me);
    for (; bMore; bMore = Module32Next(hSnapshot, &me))
    {
        if (!_stricmp(me.szModule, szDllName) || !_stricmp(me.szExePath, szDllName))
        {
            bFound = TRUE;
            break;
        }
    }
    if (!bFound)
    {
        goto label_exit_1;
    }

    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        log("OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
        result = FALSE;
        goto label_exit_1;
    }

    hModule = GetModuleHandleA("kernel32.dll");
    if (hModule) {
        pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
        if (pThreadProc) {
            hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);
            if (hThread) {
                WaitForSingleObject(hThread, INFINITE);
                CloseHandle(hThread);
                result = TRUE;
                //我们只能保证远程线程函数可以执行后返回,但是执行情况我们不知道
            }
        }
    }
    CloseHandle(hProcess);
label_exit_1:
    CloseHandle(hSnapshot);
    return result;
}
DWORD FindProcessID(CHAR* szProcessName)//
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
        if (!strcmp(szProcessName, pe.szExeFile))
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
    int key;
    char* dllPath;
    char* TargetProcess;
    DWORD dwPID;
    HANDLE hProcess;
    DWORD is_x86 = 0;
    int i, len;
    if (argc != 4)//pid button dllPath
    {
        //那就是手动输入的
#ifdef my_test
        TargetProcess = "notepad_86.exe";
        dllPath = "D:\\Projects\\test\\Dll_86.dll";
        key = 0;
        dwPID = FindProcessID(TargetProcess);
#else
        log("USAGE : TargetName dll_path 0/1\n");//说明该如何使用该工具
        return 1;
#endif
    }
    else {

        TargetProcess = argv[1];
        dllPath = argv[2];
        key = atoi(argv[3]);
        dwPID = FindProcessID(TargetProcess);
    }
    //if (!Get_Privilege(SE_DEBUG_NAME, TRUE))//提权
    //{
    //    return 1;
    //}
    if (dwPID == 0xFFFFFFFF) {
        log("no find TargetProcess\n");
        return 0;
    }
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
    IsWow64Process(hProcess, &is_x86);
    CloseHandle(hProcess);


    if (!is_x86) {
        log("Target is X64!\n");
        return 0;
    }
    len = strlen(dllPath);
    for (i = 0; i < len; i++) {
        if (dllPath[i] == '/')
        {
            dllPath = '\\';
        }
    }
    if (key) {
        if (inject_Dll(dwPID, dllPath)) {
            log("(\"%s\") success!!!\n", argv[3]);
        }
        else {
            log("(\"%s\") failed!!!\n", argv[3]);
        }
    }
    else {
        if (deject_Dll(dwPID, dllPath)) {
            log("(\"%s\") success!!!\n", argv[3]);
        }
        else {
            log("(\"%s\") failed!!!\n", argv[3]);
        }
    }
    return 0;
}