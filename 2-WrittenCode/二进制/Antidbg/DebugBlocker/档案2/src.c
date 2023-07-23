#define EVENT_SELFDBG_EVENT_NAME "SelfDebugging"
#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<stdio.h>
DWORD  IsDebugged()
{
    char wszFilePath[MAX_PATH], wszCmdLine[MAX_PATH];
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    HANDLE hDbgEvent;

    hDbgEvent = CreateEventA(NULL, FALSE, FALSE, EVENT_SELFDBG_EVENT_NAME);
    if (!hDbgEvent)
        return 0;

    if (!GetModuleFileNameA(NULL, wszFilePath, _countof(wszFilePath)))
        return 0;

    sprintf(wszCmdLine, "%s %d", wszFilePath, GetCurrentProcessId());
    if (CreateProcessA(NULL, wszCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return WAIT_OBJECT_0 == WaitForSingleObject(hDbgEvent, 0);
    }

    return 0;
}

DWORD EnableDebugPrivilege()
{
    DWORD bResult = 0;
    HANDLE hToken = NULL;
    DWORD ec = 0;

    do
    {
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
            break;

        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
            break;

        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
            break;

        bResult = 1;
    } while (0);

    if (hToken)
        CloseHandle(hToken);

    return bResult;
}

int main(int argc, char argv)
{
    if (argc < 2)
    {
        if (IsDebugged())
            ExitProcess(0);
    }
    else
    {
        DWORD dwParentPid = atoi(argv[1]);
        HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, EVENT_SELFDBG_EVENT_NAME);
        if (hEvent && EnableDebugPrivilege())
        {
            if (FALSE == DebugActiveProcess(dwParentPid))
                SetEvent(hEvent);
            else
                DebugActiveProcessStop(dwParentPid);
        }
        ExitProcess(0);
    }

    // ...

    return 0;
}