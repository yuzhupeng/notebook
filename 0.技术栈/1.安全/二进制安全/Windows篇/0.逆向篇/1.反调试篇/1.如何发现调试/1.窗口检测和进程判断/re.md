



# 父进程判断

一般情况下,我们打开一个exe可以分为以下几种情况

1), 双击打开，那么父进程是explor.exe

2), 通过cmd打开

3), 通过其它进程打开

​	A. 正常的进程打开

​	B. 非正常的，比如我们的调试器



于是我们可以通过父进程判断，或者打开的方式来确定一个进程它是不是杯调试的



通过STARTUPINFO结构体测试

测试脚本如下



## 基于 STARTUPINFO 进程创建的方式来判断



```c
#include <Windows.h>
#include <stdio.h>

int IsDebug1()
{
	STARTUPINFO si;
	DWORD* lpxx = 0;
	int i;
	/*
	DWORD   cb;
	LPSTR   lpReserved;
	LPSTR   lpDesktop;
	LPSTR   lpTitle;
	DWORD   dwX;
	DWORD   dwY;
	DWORD   dwXSize;
	DWORD   dwYSize;
	DWORD   dwXCountChars;
	DWORD   dwYCountChars;
	DWORD   dwFillAttribute;
	DWORD   dwFlags;
	WORD    wShowWindow;
	WORD    cbReserved2;
	LPBYTE  lpReserved2;
	HANDLE  hStdInput;
	HANDLE  hStdOutput;
	HANDLE  hStdError;
	*/
	GetStartupInfo(&si);
	lpxx = (DWORD*)&si;
	for (i = 0; i < 17; i++)
	{
		if (i > 0 && i < 4)
		{
			printf("%s\n", lpxx[i]);
		}
		else
		{
			printf("%x\n", lpxx[i]);
		}
		
	}
	return 0;
}

int main()
{
	IsDebug1();
	getchar();
	return 0;
}
```

然后发现无论是双击打开还是cmd启动

该结构体的wShowWindow=1

而IDA或者x64dbg打开的话,wShowWindow=0

但是用OD打开的话,wShowWindow=1

所以的话,我们可以通过这个标志位来判断是不是被类似于IDA/x64dbg打开的,虽然不是很严谨

```c
#include <Windows.h>
#include <stdio.h>

int IsDebug1()
{
	STARTUPINFO si;
	GetStartupInfo(&si);
	if (si.wShowWindow == 0)
	{
		printf("you are debug me!\n");
	}
	else
	{
		printf("it is fine!\n");
	}
	
	return 0;
}

int main()
{
	IsDebug1();
	getchar();
	return 0;
}
```



## 父进程检测 判断

这个东西的话,我们就得判断特定的进程了

比如说我们可以获取当前进程的父进程

可是父进程是谁,我们得好好的判断一下

常见的调试器进程

```
ida.exe
ida64.exe
x32dbg.exe
x64dbg.exe
OllyICE.exe
...
```

其实只要调试器的exe换一个名字的话

那么就绕过了。。。。

所以，具体的的代码其实意义不大



形式1  

```c
#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>

int IsDebug()
{
	DWORD ExplorerId = 0;
	PROCESSENTRY32 pe32 = { 0 };
	DWORD ProcessId = GetCurrentProcessId();//获取当前进程的ID

	GetWindowThreadProcessId(FindWindowA("Progman", NULL), &ExplorerId);//获取Progman主线程的ID

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);//开始遍历所有进程
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hProcessSnap, &pe32);
		do
		{	
			if (ProcessId == pe32.th32ProcessID)//如果是当前进程
			{	// 判断父进程是否是 Explorer.exe
				if (pe32.th32ParentProcessID != ExplorerId)//查看当前进程的父进程是不是Progman
				{	 
					HANDLE h_process = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ParentProcessID);
					TerminateProcess(h_process, 0);//如果不是,就中止当前进程
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



形式2,  

```c
#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#
int main(int argc, char* argv[])
{
	DWORD	dwPid;
	DWORD	dwParentPid = 0;
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





# 当前所有窗口名字检测

既然是对所有窗口名字检测,什么了计算我们不调试当前的exe

也不能打开调试器,有点盲目



绕过的办法

1), 修改窗口注册的类名(写死了,无法修改)

2), 修车的exe调试器名字



形式1

```c
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


	// 通过注册窗口的类名来判断
	if (FindWindow("OllyDbg", NULL) || FindWindow("TIdaWindow", NULL) || FindWindow("WinDbgFrameClass", NULL))
	{
		printf("1 Found a debugger window!!!\n");
	}

	// 寻找所有窗口的名字
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
	system("pause null");
	return 0;
}
```



形式2

```c
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
		if (strcmp(pe32.szExeFile, "OllyDBG.EXE") == 0 || 
			strcmp(pe32.szExeFile, "OllyICE.exe") == 0 || 
			strcmp(pe32.szExeFile, "x64_dbg.exe") == 0 || 
			strcmp(pe32.szExeFile, "windbg.exe") == 0 || 
			strcmp(pe32.szExeFile, "ImmunityDebugger.exe") == 0)
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









# 当前活动窗口的进程名检测(不是很好用)

绕过的方法也是修改名字



下面这个例子是基于父进程的运行环境(乱说)

比如参数会出现

```
IDA - C1.exe E:\Code\normal\C\2022\tmpCTF\Debug\C1.exe //那么就检测字符串IDA
0x0019fa08 "tmpCTF (正在调试) - Microsoft Visual Studio"

C2_default_main.exe - PID: 20728 - 模块: c2_default_main.exe - 线程: 主线程 8612 - x64dbg
```

```c
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
