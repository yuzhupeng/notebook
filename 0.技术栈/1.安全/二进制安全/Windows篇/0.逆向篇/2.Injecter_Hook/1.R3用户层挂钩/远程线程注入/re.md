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



具体分析如下



## main分析



首先我们的参数如下

```c++
注入器.exe 注入目标的进程命 待注入的dll文件 1/0
```

1是注入dll,0是卸载已经注入的dll



然后我们获取目标进程的PID

```c++
TargetProcess = argv[1];
dllPath = argv[2];
key = atoi(argv[3]);
dwPID = FindProcessID(TargetProcess);
```



然后判断目标进程是x86还是x64的

```c++
hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
IsWow64Process(hProcess, &is_x86);
CloseHandle(hProcess);


if (!is_x86) {
    log("Target is X64!\n");
    return 0;
}
```



x86注入器注入x86的进程

x64的注入器注入x64的进程

我目前只能这样,,以后遇到更好的再去说吧

ps: 总不能说是x86加载x64的dll吧

好像之前写过,x64可以注入x86,但是x86无法注入x64的



然后对路径做一个处理

```c++
for (i = 0; i < len; i++) {
    if (dllPath[i] == '/')
    {
        dllPath = '\\';
    }
}
```

因为Windows用的是`\`



然后就是一个注入的过程



## 注入器分析





首先是打开进程句柄

然后在远程进程创建一块内存,用于存放dllPath的路径

然后远程开一个线程

线程的执行函数是LoadLibraryA,,,(所有x86进程的kernel32的API地址是一样的)

函数参数是远程进程内存里面的DLlPath

然后就让远程线程跑起来

然后就over了,哈哈

```c++
BOOL inject_Dll(DWORD dwPID,char* szDllPath)
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
		VirtualFreeEx(hProcess,pRemoteBuf, dwBufSize,MEM_FREE);
	}
	CloseHandle(hProcess);
	return TRUE;
}
```





# 注入FreeLibrary卸载指定API

通过之前的例子,我想我们应该已经很了解了一个注入过程

注入FreeLibrary一样也是注入的

但是FreeLibrary的参数是dll的加载地址

所以我们需要知道目标进程的dll模块基地址才能卸载的



下面这是一个简单的自身模块的遍历,找打目标进程的模块基地址

```c++

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
```



因为FreeLivrary的参数是一个dll基地址

dll基地址就是一块内存的起点,这个东西在远程进程本来就存在

所以就自然不会存在什么远程进程的内存开辟什么的



之后的东西和以前是一样的

```c++
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
		result=FALSE;
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
```





# 实验



