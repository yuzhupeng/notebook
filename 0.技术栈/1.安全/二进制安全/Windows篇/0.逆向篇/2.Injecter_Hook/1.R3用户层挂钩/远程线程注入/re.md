# 前言



> 远程线程注入认为也叫代码注入



期间用到的API

```c++
VirtualAllocEx;
WriteProcessMemory;
CreateRemoteThread;
VirtualFreeEx;
```

上面这些API的作用是在远程进程分配一块内存

内存的熟悉可以自定义配置

然后CreateRemoteThread然远程进程跑一个我们指定地址的线程



然后与之配合的API

```c++
GetProcAddress;
LoadLibrary;
```

用到的提权代码,注入好像好像需要一些权限才可以注入的

具体的提权过程目前不需要特别去关心,,只需要指定调用即可

# 提权的简单分析(了解)



在提权过程中，通常需要使用以下三个函数来完成：

1. **`OpenProcessToken`**: 这个函数用于打开进程的访问令牌（Access Token）。

   访问令牌是一种用于标识和验证进程身份和权限的数据结构。**通过打开进程的访问令牌，可以获取到对该进程进行权限修改的权限。**

2. **`LookupPrivilegeValue`**: 这个函数用于查找指定特权（Privilege）的特权标识符（LUID）。特权标识符是一种**唯一的数值，用于表示系统中的不同特权**。在提权过程中，需要使用特权标识符来标识要修改的特权。

3. **`AdjustTokenPrivileges`**: 这个函数用于调整访问令牌（Access Token）中的特权。**通过提供特权标识符和特权的状态信息，可以将指定的特权授予或撤销访问令牌**。在提权过程中，需要使用该函数来修改访问令牌中的特权，以获取所需的权限。

下面是它们的配合过程：

1. 首先，使用 **`OpenProcessToken`** 打开目标进程的访问令牌，以获取对其进行修改的权限。
2. 然后，使用 **`LookupPrivilegeValue`** 查找指定特权的特权标识符（LUID），通常是 **`SE_DEBUG_NAME`** 特权。
3. 接下来，创建一个 **`TOKEN_PRIVILEGES`** 结构，将要修改的特权标识符和特权的状态信息填充进去。
4. 最后，使用 **`AdjustTokenPrivileges`** 函数将特权授权或撤销给访问令牌。函数返回成功表示特权调整成功。

这样，通过配合使用这三个函数，可以在提权过程中打开目标进程的访问令牌，查找特权标识符，并将特权授权给访问令牌，以获取所需的权限。

我的小结:

获取令牌的Token : **`OpenProcessToken`**

查找特权的uuid : **`LookupPrivilegeValue`**

开一个新的权限结构体 `TOKEN_PRIVILEGES` ,然后根据已有的或者想要的给它初始化

然后把新的权限结构体注入到指定进程的令牌中 **`AdjustTokenPrivileges`**

关于访问令牌的一些介绍:

访问令牌（access token）是一个数据结构，用于在 Windows 操作系统中标识和验证身份认证。

每个进程都有一个访问令牌，该令牌用于标识该进程所属的用户或用户组，并规定了该进程可以执行的操作。

某些操作需要特殊的权限才能执行，这些权限被授予给某些用户或用户组，

当一个进程需要执行这些需要特殊权限的操作时,就需要使用访问令牌来检查其是否具有足够的权限。

因此，访问令牌在 Windows 操作系统中扮演着至关重要的角色，能够保障系统和应用程序的安全性和稳定性。





# 注入LoadLibrary加载指定API

这种注入的优点是不需要去注入自定义shellcode

而是注入kernel32自带的函数的字节码

并且kernel32在大多数的进程中

API的位置是固定的,,,

然后我们给他传递参数,他就可以加载我们指定的dll文件



具体情况r如下

```c++
#include<windows.h>
#include<stdio.h>
#include<string.h>

//#define my_test 1
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

	if (!LookupPrivilegeValue(//该函数用于检索给定特权名称的特权标识符
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
	hMod = GetModuleHandleA("kernel32.dll");//因为一定会被
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);//创建一个远程线程,指定了线程处理函数,线程参数(参数的地址是刚才在目标进程分配的)
	WaitForSingleObject(hThread, INFINITE);//等待远程线程执行完毕

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}
```







