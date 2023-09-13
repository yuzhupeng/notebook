#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

DWORD is_x86;
HANDLE hProcess;

typedef void (*type_Set_TagetProcess)(WCHAR* szProcessName);

enum {INJECTION_MODE = 0, EJECTION_MODE};

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) 
{
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if( !OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,  &hToken) )
    {
        printf("OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }

    if( !LookupPrivilegeValue(NULL,            // lookup privilege on local system
                              lpszPrivilege,   // privilege to lookup 
                              &luid) )        // receives LUID of privilege
    {
        printf("LookupPrivilegeValue error: %u\n", GetLastError() ); 
        return FALSE; 
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if( bEnablePrivilege )
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.
    if( !AdjustTokenPrivileges(hToken, 
                               FALSE, 
                               &tp, 
                               sizeof(TOKEN_PRIVILEGES), 
                               (PTOKEN_PRIVILEGES) NULL, 
                               (PDWORD) NULL) )
    { 
        printf("AdjustTokenPrivileges error: %u\n", GetLastError() ); 
        return FALSE; 
    } 

    if( GetLastError() == ERROR_NOT_ALL_ASSIGNED )
    {
        printf("The token does not have the specified privilege. \n");
        return FALSE;
    } 

    return TRUE;
}
/*
* 函数名: InjectDll_hook
* 函数参数:
*   参数1: 目标进程的PID
*   参数2: 要注入的dll路径
* 函数功能:
    线程创建线程加载我们的dll
*/
BOOL InjectDll_hook(DWORD dwPID, CHAR*szDllPath)
{
    HANDLE hProcess, hRemoteThread;
    LPVOID pRemoteBuf;
    DWORD dwBufSize = strlen(szDllPath) + 1;
    LPTHREAD_START_ROUTINE  pThreadProc;
    DWORD64 exitCode = 0;
    if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        printf("OpenProcess failed");
        return FALSE;
    }

    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    if (pRemoteBuf)
    {
        WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);

        pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);

        //WaitForSingleObject(hRemoteThread, INFINITE);
        WaitForSingleObject(hRemoteThread, 3000);//有些可能报错,一直不返回?
        GetExitCodeThread(hRemoteThread, &exitCode);
        //wprintf(L" %p ", exitCode);
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hRemoteThread);
    }
    CloseHandle(hProcess);
    if (exitCode)
        return TRUE;
    else
        return FALSE;
}
BOOL EjectDll_unhook(DWORD dwPID, CHAR* szDllPath)
{
    BOOL bMore = FALSE, bFound = FALSE;
    HANDLE hSnapshot, hProcess, hRemoteThread;
    MODULEENTRY32 me = { sizeof(me) };
    LPTHREAD_START_ROUTINE  pThreadProc;
    DWORD64 exitCode = 0;
    //拍快照?
    if (INVALID_HANDLE_VALUE == (hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID)))
    {
        return FALSE;
    }

    //寻找目标进程的dll是否加载,加载了就卸载
    bMore = Module32First(hSnapshot, &me);
    for( ; bMore ; bMore = Module32Next(hSnapshot, &me) )
    {
        if( !strcmp(me.szModule, szDllPath) || !strcmp(me.szExePath, szDllPath) )
        {
            bFound = TRUE;
            break;
        }
    }

    if( !bFound )
    {
        CloseHandle(hSnapshot);
        return FALSE;
    }

    if( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        printf("OpenProcess failed");
        CloseHandle(hSnapshot);
        return FALSE;
    }

    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");
    if (pThreadProc)
    {
        hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);
        WaitForSingleObject(hRemoteThread, INFINITE);
        GetExitCodeThread(hRemoteThread, &exitCode);
    }    
    CloseHandle(hRemoteThread);
    CloseHandle(hProcess);
    CloseHandle(hSnapshot);
    if (exitCode)
        return TRUE;
    else
        return FALSE;
}
BOOL InjectAllProcess(int nMode, CHAR* szDllPath)
{
    DWORD                   dwPID = 0;
    HANDLE                  hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32          pe;

    // Get the snapshot of the system
    pe.dwSize = sizeof( PROCESSENTRY32 );



    //TMD  CreateToolhelp32Snapshot会间接调用 NtQuerySystemInformation 同时
    hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, 0 );
    
    // 遍历所有进程
    Process32First(hSnapShot, &pe);
    printf("|---BEGIN---|\n");
    do
    {
        dwPID = pe.th32ProcessID;
        printf("%d:%s", dwPID, pe.szExeFile);
 
        //对PID<100的进程不注入 (考虑到安全性)
        if (dwPID < 100)
        {
            printf(" dangerous process");
            goto  label_OpenProcessfailed;
        }
        //dwPID = FindProcessID(L"procexp64.exe");
        if (!(hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID)))
        {
            printf(" OpenProcess  failed");
            goto  label_OpenProcessfailed;
        }
        IsWow64Process(hProcess, &is_x86);
        CloseHandle(hProcess);
        if (is_x86)
        {
            printf(" x86 process");
        }
        else
        {
            if (nMode == INJECTION_MODE)
            {
                if (InjectDll_hook(dwPID, szDllPath))
                {
                    printf(" sth success");
                }
                else
                {
                    printf(" sth failed");
                }
            }
            else
            {
                if (EjectDll_unhook(dwPID, szDllPath))
                {
                    printf(" sth success");
                }
                else
                {
                    printf(" sth failed");
                }
            }
        }
 label_OpenProcessfailed:
        printf("\n");     
    }
    while( Process32Next(hSnapShot, &pe) );
    printf("|----END----|\n");

    CloseHandle(hSnapShot);
    return TRUE;
}
DWORD FindProcessID(WCHAR* szProcessName)//
{
    DWORD dwPID = 0xFFFFFFFF;
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32W pe;

    // Get the snapshot of the system
    pe.dwSize = sizeof(PROCESSENTRY32W);
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

    // find process
    Process32FirstW(hSnapShot, &pe);
    do
    {
        if (!wcscmp(szProcessName, pe.szExeFile))

        {
            dwPID = pe.th32ProcessID;
            break;
        }
    } while (Process32NextW(hSnapShot, &pe));

    CloseHandle(hSnapShot);

    return dwPID;
}


int testAll()
{
    int nMode = INJECTION_MODE;
    HMODULE hLib = NULL;
    type_Set_TagetProcess Set_TagetProcess = NULL;
    CHAR* szmode;
    CHAR* dllPath;
    WCHAR* processName;

 
    szmode = "-hide";
    //szmode = L"-show";
    dllPath = "c:\\redqx64.dll";
    processName = L"notepad.exe";
    
    //  常规提权
    if (SetPrivilege(SE_DEBUG_NAME, TRUE))
    {
        // load library
        hLib = LoadLibraryA(dllPath);//自己去加载那个dll,同时自己也被hook了
        if (hLib)
        {
            // set process name to hide
            Set_TagetProcess = (type_Set_TagetProcess)GetProcAddress(hLib, "Set_TagetProcess");//获取dllname
            if (Set_TagetProcess)
            {
                Set_TagetProcess(processName);//会把数据复制到dll里的某个全局变量中

                // Inject(Eject) Dll to all process
                if (!strcmp(szmode, "-show"))
                {
                    nMode = EJECTION_MODE;
                }

                //往所有进程注入.. 
                //有个问题: x64的dll只能往x64注入
                //算了吧 所有进程注入,系统就蓝屏了
                InjectAllProcess(nMode, dllPath);
            }
            FreeLibrary(hLib);
        }
    }
    return 0;
}
void test_Single()
{
    int nMode = INJECTION_MODE;
    HMODULE hLib = NULL;
    HANDLE hProcess;
    type_Set_TagetProcess Set_TagetProcess = NULL;
    CHAR* szmode;
    CHAR* dllPath;
    WCHAR* processName;
    DWORD dwPID;



    szmode = "-hide";
   // szmode = "-show";
    dllPath = "c:\\redqx64.dll";
    processName = L"notepad.exe";

    //  常规提权
    if (SetPrivilege(SE_DEBUG_NAME, TRUE))
    {
        // load library
        hLib = LoadLibraryA(dllPath);//自己去加载那个dll,同时自己也被hook了
        if (hLib)
        {
            // set process name to hide
            Set_TagetProcess = (type_Set_TagetProcess)GetProcAddress(hLib, "Set_TagetProcess");//获取dllname
            if (Set_TagetProcess)
            {
                Set_TagetProcess(processName);//会把数据复制到dll里的某个全局变量中

                // Inject(Eject) Dll to all process
                if (!strcmp(szmode, "-show"))
                {
                    nMode = EJECTION_MODE;
                }

                dwPID = FindProcessID(L"SearchApp.exe");

                if (!(hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID)))
                {
                    printf(" OpenProcess  failed");
                    return;
                }
                IsWow64Process(hProcess, &is_x86);
                CloseHandle(hProcess);
                if (is_x86)
                {
                    if (nMode == INJECTION_MODE)
                    {
                        if (InjectDll_hook(dwPID, dllPath))
                        {
                            printf(" sth success");
                        }
                        else
                        {
                            printf(" sth failed");
                        }
                    }
                    else
                    {
                        if (EjectDll_unhook(dwPID, dllPath))
                        {
                            printf(" sth success");
                        }
                        else
                        {
                            printf(" sth failed");
                        }
                    }
                }

            }
            FreeLibrary(hLib);
        }
    }
    return ;
}
int main(int argc, CHAR* argv[])
{
    testAll();
    //test_Single();
    return;
}
