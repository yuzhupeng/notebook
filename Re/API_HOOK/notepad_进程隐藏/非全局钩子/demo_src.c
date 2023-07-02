#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

DWORD is_x86;
HANDLE hProcess;

typedef void (*t_SetProcName)(WCHAR* szProcName);

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
BOOL InjectDll_hook(DWORD dwPID, WCHAR*szDllPath)
{
	HANDLE hProcess, hRemoteThread;
	LPVOID pRemoteBuf;
	DWORD dwBufSize = (DWORD)(wcslen(szDllPath) + 1) * sizeof(TCHAR);//一个wchar是2字节
	LPTHREAD_START_ROUTINE  pThreadProc;
    DWORD64 exitCode = 0;
	if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        wprintf(L"OpenProcess failed");
		return FALSE;
    }

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    if (pRemoteBuf)
    {
        WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);

        pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
        hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);

        WaitForSingleObject(hRemoteThread, INFINITE);
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
BOOL EjectDll_unhook(DWORD dwPID, WCHAR* szDllPath)
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
	bMore = Module32FirstW(hSnapshot, &me);
	for( ; bMore ; bMore = Module32NextW(hSnapshot, &me) )
	{
		if( !_wcsicmp(me.szModule, szDllPath) || !_wcsicmp(me.szExePath, szDllPath) )
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
        wprintf(L"OpenProcess failed");
		CloseHandle(hSnapshot);
		return FALSE;
	}

	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "FreeLibrary");
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

BOOL InjectAllProcess(int nMode, WCHAR* szDllPath)
{
	DWORD                   dwPID = 0;
	HANDLE                  hSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32          pe;

	// Get the snapshot of the system
	pe.dwSize = sizeof( PROCESSENTRY32 );



    //TMD  CreateToolhelp32Snapshot会间接调用 NtQuerySystemInformation 同时
	hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, 0 );
	
    // 遍历所有进程
	Process32FirstW(hSnapShot, &pe);
    wprintf(L"|---BEGIN---|\n");
	do
	{
        Sleep(100);
		dwPID = pe.th32ProcessID;
        wprintf(L"%d:%s", dwPID, pe.szExeFile);
        if (!wcscmp(L"SearchApp.exe", pe.szExeFile) || !wcscmp(L"SearchApp.exe","backgroundTaskHost.exe"))
        {
            goto  label_OpenProcessfailed;
        }

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
            wprintf(L" x86 process");
        }
        else
        {
            if (nMode == INJECTION_MODE)
            {
                if (InjectDll_hook(dwPID, szDllPath))
                {
                    wprintf(L" sth success");
                }
                else
                {
                    wprintf(L" sth failed");
                }
            }
            else
            {
                if (EjectDll_unhook(dwPID, szDllPath))
                {
                    wprintf(L" sth success");
                }
                else
                {
                    wprintf(L" sth failed");
                }
            }
        }
 label_OpenProcessfailed:
        wprintf(L"\n");     
	}
	while( Process32NextW(hSnapShot, &pe) );
    wprintf(L"|----END----|\n");

	CloseHandle(hSnapShot);
	return TRUE;
}
DWORD FindProcessID(WCHAR* szProcessName)//
{
    DWORD dwPID = -1;
    // SYSTEM_INFO systemInfo;//用于判断进程是x86还是x64的
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;

    // Get the snapshot of the system
    pe.dwSize = sizeof(PROCESSENTRY32);
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

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
int main(int argc, char* argv[])
{
    int nMode = INJECTION_MODE;
    HMODULE hLib = NULL;
    t_SetProcName SetProcName = NULL;
    WCHAR* szmode;
    WCHAR* dllPath;
    WCHAR* processName;
 
    
    if (0)
    {
        if (argc != 4)
        {
            printf("\n Usage  : HideProc.exe arg1:<-hide|-show> arg2:<process name> arg3:<dll path>\n\n");// 1 2 3
            return 1;
        }
        //szmode = argv[2];
        //dllPath = argv[3];
    }
    else
    {
        szmode=L"-hide";
        //szmode = L"-show";
        dllPath=L"c:\\redqx_x64.dll";
        processName=L"notepad.exe";
    }
	//  常规提权
    if (SetPrivilege(SE_DEBUG_NAME, TRUE))
    {
        // load library
        hLib = LoadLibraryW(dllPath);//自己去加载那个dll,同时自己也被hook了
        wprintf(L"me: %p", hLib);
        if (hLib)
        {
            // set process name to hide
            SetProcName = (t_SetProcName)GetProcAddress(hLib, "SetProcName");//获取dllname
            if (SetProcName)
            {
                SetProcName(processName);//会把数据复制到dll里的某个全局变量中

                // Inject(Eject) Dll to all process
                if (!_wcsicmp(szmode, L"-show"))
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
