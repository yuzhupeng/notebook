# 入门1

分析的demo来着逆向工程核心原理page.211

书上源码

```c
// dllmain.cpp : 定义 DLL 应用程序的入口点。
#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
// myhack2.cpp

#include "windows.h"
#include "tchar.h"

#define DEF_CMD  L"c:\\Program Files\\Internet Explorer\\iexplore.exe" 
#define DEF_ADDR L"http://www.naver.com"
#define DEF_DST_PROC L"notepad.exe"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    WCHAR szCmd[MAX_PATH] = { 0, };
    WCHAR szPath[MAX_PATH] = { 0, };
    WCHAR* p = NULL;
    STARTUPINFOW si = { 0, };
    PROCESS_INFORMATION pi = { 0, };

    si.cb = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        if (!GetModuleFileNameW(NULL, szPath, MAX_PATH))
            break;

        if (!(p = wcsrchr(szPath, L'\\')))
            break;

        if (wcscmp(p + 1, DEF_DST_PROC))
            break;

        wsprintfW(szCmd, L"%s %s", DEF_CMD, DEF_ADDR);
        if (!CreateProcessW(NULL, szCmd,
            NULL, NULL, FALSE,
            NORMAL_PRIORITY_CLASS,
            NULL, NULL, &si, &pi))
            break;

        if (pi.hProcess != NULL)
            CloseHandle(pi.hProcess);

        break;
    }

    return TRUE;
}
```

dllmian函数其实就是判断当前进程是不是notepad,如果是的话,就浏览器打开某个页面