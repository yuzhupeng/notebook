# windows层面反调试

# 拦截的API

**EnumWindows() and SuspendThread()**这个技术的想法是暂停父进程的自有线程。

**SwitchDesktop()**

Windows支持每个会话有多个桌面。可以选择一个不同的活动桌面，

其效果是隐藏了之前活动桌面的窗口，

而且没有明显的方法可以切换回旧的桌面。

此外，来自被调试进程桌面的鼠标和键盘事件将不再被传递给调试器，

因为它们的来源不再被共享。这显然使调试变得不可能

# explorer

我们知道一般情况下都是通过Windows的资源管理器来打开程序，所

以一般的手动打开的进程父进程都是explorer.exe，如果通过调试器创建进程则父进程就不会是explorer.exe。

所以可以通过检测父进程是否是explorer.exe来检测调试器的存在。

通过这个标志我们就可以获得**父进程ID**(Reserved3)，

后续在通过OpenProcess获取父进程句柄，调用GetProcessImageFileName获得父进程名

其实如果你用在文件夹里面打开程序,就不是反调试

用其它方式打开就属于在调试(哪怕用cmd打开就是反调试)

这个方法有点小问题

```
#include <Windows.h>
#include <stdio.h>

int IsDebug()
{
	STARTUPINFO si = { 0 };
	GetStartupInfo(&si);

	if (si.dwFlags != 1)
		return 1;
	return 0;
}

int main(int argc, char* argv[])
{
	if (IsDebug())
		printf("Find You \n");
	else
		printf("where are you\n");

	system("pause");
	return 0;
}
```

失败的案例

explorer.exe创建进程的时候会把STARTUPINFO结构中的值设为0，

而非explorer.exe创建进程的时候会忽略这个结构中的值，也

就是结构中的值不为0。所以可以利用STARTUPINFO来判断程序是否在被调试

```
#include <windows.h>
#include <stdio.h>
BOOL CheckDebug()
{
    STARTUPINFO si;
    GetStartupInfo(&si);
    if (si.dwX != 0 || si.dwY != 0 || si.dwFillAttribute != 0 || si.dwXSize != 0 || si.dwYSize != 0 || si.dwXCountChars != 0 || si.dwYCountChars != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
int main()
{
    if (CheckDebug())
    {
        printf("Get You\n");
    }
    else
    {
        printf("where are you\n");
    }
    return 0;
}
```

# 注册表(失败)

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#define DBG_PRINTEXCEPTION_WIDE_C 0x4001000A

BOOL CheckDebug()
{
    BOOL is_64;
    IsWow64Process(GetCurrentProcess(), &is_64);
    HKEY hkey = NULL;
    char key[] = "Debugger";
    char reg_dir_32bit[] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug";
    char reg_dir_64bit[] = "SOFTWARE\\Wow6432Node\\Microsoft\\WindowsNT\\CurrentVersion\\AeDebug";
    DWORD ret = 0;
    if (is_64) {
        ret = RegCreateKeyA(HKEY_LOCAL_MACHINE, reg_dir_64bit, &hkey);
    }
    else {
        ret = RegCreateKeyA(HKEY_LOCAL_MACHINE, reg_dir_32bit, &hkey);
    }
    if (ret != ERROR_SUCCESS) {
        return FALSE;
    }
    char tmp[256];
    DWORD len = 256;
    DWORD type;
    ret = RegQueryValueExA(hkey, key, NULL, &type, (LPBYTE)tmp, &len);
    if (strstr(tmp, "OllyIce") != NULL || strstr(tmp, "OllyDBG") != NULL || strstr(tmp, "WinDbg") != NULL || strstr(tmp, "x64dbg") != NULL || strstr(tmp, "Immunity") != NULL)
    {
        return TRUE;
    }
    else {
        return FALSE;
    }
}
int main()
{
    printf("ok?\n");
    printf("%d", CheckDebug());
    return 0;
}
```

# 当前窗口检测

检测一些IDA进程,IDA文件,IDA窗口,IDA注册表,主机名.计算机名,用户名,环境变量来决定反调试你

但是有个问题..如果你没有调试当前程序,然后也开了IDA之类的工具,那么的话也会被视为反调试

`FindWindow ,GetWindowText`

```
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include <windows.h>

void FindDebuggerWindow();

void FindDebuggerWindow()
{
    BOOL bDebugging = FALSE;
    char szWindow_path[MAX_PATH] = { 0 };
    HWND hWnd = GetDesktopWindow();
    hWnd = GetWindow(hWnd, GW_CHILD);
    hWnd = GetWindow(hWnd, GW_HWNDFIRST);
    // using ClassName
    if (FindWindow("OllyDbg", NULL) || FindWindow("TIdaWindow", NULL) || FindWindow("WinDbgFrameClass", NULL))
    {
        printf("1 Found a debugger window!!!\n");
    }

    // using WindowName
    bDebugging = FALSE;
    while (hWnd)
    {
        if (GetWindowText(hWnd, szWindow_path, MAX_PATH))
        {
            if (strstr(szWindow_path, "IDA") ||
                strstr(szWindow_path, "OllyDbg") ||
                strstr(szWindow_path, "WinDbg"))
            {
                printf("2 Found a debugger window!!!\n\n");
                break;
            }
        }
        hWnd = GetWindow(hWnd, GW_HWNDNEXT);//可能这是一个遍历
    }
}
int main()
{
    FindDebuggerWindow();
    system("pause nul");
    return 0;
}
```

下面这个例子是基于进程快照的

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <tlhelp32.h>

BOOL CheckDebug()
{
    DWORD ID;
    DWORD ret = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    BOOL bMore = Process32First(hProcessSnap, &pe32);
    while (bMore)
    {
        if (strcmp(pe32.szExeFile, "OllyDBG.EXE") == 0 || strcmp(pe32.szExeFile, "OllyICE.exe") == 0 || strcmp(pe32.szExeFile, "x64_dbg.exe") == 0 || strcmp(pe32.szExeFile, "windbg.exe") == 0 || strcmp(pe32.szExeFile, "ImmunityDebugger.exe") == 0)
        {
            return TRUE;
        }
        bMore = Process32Next(hProcessSnap, &pe32);
    }
    CloseHandle(hProcessSnap);
    return FALSE;
}
int main()
{
    printf("%d", CheckDebug());
    return 0;
}
```

用了另外一个函数直接获取句柄`GetForegroundWindow()`

下面这个例子是基于父进程的运行环境(乱说)

比如参数会出现

```
IDA - C1.exe E:\Code\normal\C\2022\tmpCTF\Debug\C1.exe //那么就检测字符串IDA
0x0019fa08 "tmpCTF (正在调试) - Microsoft Visual Studio"
```

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

BOOL CheckDebug()
{
    char fore_window[1024];
    GetWindowTextA(GetForegroundWindow(), fore_window, 1023);
    if (strstr(fore_window, "WinDbg") != NULL
        || strstr(fore_window, "x64_dbg") != NULL
        || strstr(fore_window, "OllyICE") != NULL
        || strstr(fore_window, "OllyDBG") != NULL
        || strstr(fore_window, "Immunity") != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
int main()
{
    printf("%d", CheckDebug());
    return 0;
}
```

# 父进程监测

下面这个代码,有点小厉害

原理:

遍历进程ID,然后找到自己,然后去获取父进程ID

如果父进程不是标准的进程,就杀死该进程

如果父进程是标准的进程,就继续

```
#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>

int IsDebug()
{
	DWORD ExplorerId = 0;
	PROCESSENTRY32 pe32 = { 0 };
	DWORD ProcessId = GetCurrentProcessId();

	GetWindowThreadProcessId(FindWindow("Progman", NULL), &ExplorerId);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hProcessSnap, &pe32);
		do
		{	// 先判断是不是我们自己进程的PID
			if (ProcessId == pe32.th32ProcessID)
			{	// 判断父进程是否是 Explorer.exe
				if (pe32.th32ParentProcessID != ExplorerId)
				{	// 如果被调试器附加了,我们直接强制干调调试器
					HANDLE h_process = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ParentProcessID);
					TerminateProcess(h_process, 0);
					return 1;
				}
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	return 0;
}

int main(int argc, char* argv[])
{
	if (IsDebug())
	{
		printf("进程正在被调试 \n");
	}
	else
	{
		printf("where are you\n");
	}
	system("pause");
	return 0;
}
```

下面这个代码同理,也就是检测父进程是不是规定的那几个

```
explorer.exe
cmd.exe
services.exe
```

```
#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#
int main(int argc, char* argv[])
{
	DWORD	dwPid;
	DWORD	dwParentPid=0;
	DWORD	dwPidExplorer = 0;
	DWORD	dwPidCmd = 0;
	DWORD	dwPidServices = 0;
	HANDLE	hSnapProcess;
	DWORD	dwFlag = 0;

	dwPid = GetCurrentProcessId();
	hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32	pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (hSnapProcess != INVALID_HANDLE_VALUE)
	{
		Process32First(hSnapProcess, &pe32);
		do {
			if (pe32.th32ProcessID == dwPid)
				dwParentPid = pe32.th32ParentProcessID;
			if (lstrcmp(pe32.szExeFile, "explorer.exe") == 0)
				dwPidExplorer = pe32.th32ProcessID;
			if (lstrcmp(pe32.szExeFile, "cmd.exe") == 0)
				dwPidCmd = pe32.th32ProcessID;
			if (lstrcmp(pe32.szExeFile, "services.exe") == 0)
				dwPidServices = pe32.th32ProcessID;
		} while (Process32Next(hSnapProcess, &pe32));

		if (dwParentPid == dwPidExplorer)
			dwFlag = 1;
		else if (dwParentPid == dwPidCmd)
			dwFlag = 1;
		else if (dwParentPid == dwPidServices)
			dwFlag = 1;

	}
	else
	{
		CloseHandle(hSnapProcess);
		return 0;
	}

	if (dwFlag == 1)
		MessageBox(NULL, "程序正常运行！", NULL, MB_OK);
	else if (dwFlag == 0)
		MessageBox(NULL, "已检测到调试器！", NULL, MB_OK);
	return 0;
}
```

# SeDebugPrivilege权限(失败)

默认情况下进程没有`SeDebugPrivilege`权限的,但是调试器有`SeDebugPrivilege`权限,

当调试进程被加载时就继承了`SeDebugPrivilege`权限,对`SeDebugPrivilege`权限的判断

可以用能否打开`csrss.exe`进程来判断.

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <tlhelp32.h>

DWORD CheckDebug();
DWORD NormallCode(int x, int y);

int main()
{
    NormallCode(1, 2);
    if (CheckDebug())
    {
        printf("Get You\n");
    }
    else
    {
        printf("where are you\n");
    }
    NormallCode(3, 4);
    return 0;
}
DWORD CheckDebug()
{
    DWORD ID = 0;
    DWORD ret = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    BOOL bMore = Process32First(hProcessSnap, &pe32);
    while (bMore)
    {
        if (strcmp(pe32.szExeFile, "csrss.exe") == 0)
        {
            ID = pe32.th32ProcessID;
            break;
        }
        bMore = Process32Next(hProcessSnap, &pe32);
    }
    CloseHandle(hProcessSnap);
    if (OpenProcess(PROCESS_QUERY_INFORMATION, NULL, ID) != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
DWORD NormallCode(int x, int y)
{
    int z = x + y;
    z = z ^ x;
    z = z ^ y;
    z = z | (x + y);
    return z;
}
```

# 行为占用

行为占用是指在需要保护的程序中,程序自身将一些只能同时有`1`个实例的功能占为己用.

比如一般情况下,一个进程只能同时被`1`个调试器调试,那么就可以将程序以调试方式启动,然后利用系统的调试机制防止被其他调试器调试.

例如:`ptrace(PTRACE_TRACEME, 0, 0, 0)`

但是对于ELF层面当然是可以的

对于EXE层面,我还没有涉及