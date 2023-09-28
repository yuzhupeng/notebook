# res/eg3

以`../res/eg3`为例

反正就那些流程啦, 启动调试程序,进入调试的消息循环



main函数如下

```c
int main()
{
    HANDLE hMutex = NULL;
    //这个互斥体的创建是用于区分进程的,因为该互斥体是位于进程之外的东西
    if (!(hMutex = CreateMutex( //互斥体创建
        NULL, //安全属性,垃圾,NULL
        FALSE, //互斥信号不属于任何人
        DEF_MUTEX_NAME //互斥的字符串名字,随便取
    )))
    {
        printf("CreateMutex Failed [dec:%d]\n", GetLastError());// 创建mutex失败
        return;
    }

    // 检查mutex，判断子进程和父进程
    if (ERROR_ALREADY_EXISTS != GetLastError())
    {
        Father_process();
    }
    else
    {
        Child_process();//因为相同的互斥体不能创建2次,第二次创建的肯定是子进程
    }
    return 0;
}
```



启动的时候,根据条件,兵分二路

这里的条件就是一个互斥体,相同的互斥体不可以创建2次

ps: 如果是父进程,就会先创建一个互斥体



看一下子进程代码

```c
void Child_process()
{
	MessageBox(NULL, "son", "redqx", MB_OK);
	int i = 0;
	char flag[] =
	{
		0x74,0x40,0x57,0x43,
		0x6b,0x62,0x77,0x66,
		0x65,0x6a,0x21,0x76,
		0x21,0x95,0x77,0x9c,
		0x58,0x92,0x5d,0x93,
		0x65,0x8d,0
	};
	// 下面是引发异常的汇编,自己可以DIY
	__asm
	{
		xor eax, eax
		mov dword ptr[eax], 0
	}
	for (i = 0; i < 22; i++)
	{
		if (i & 1 == 1)
		{
			flag[i] = (flag[i] - 0x20) ^ 0x10;
		}
		else
		{
			flag[i] = (flag[i] - 0x10) ^ 0x20;
		}
		printf("%c", flag[i]);
	}
	MessageBox(NULL, flag, "Good Boy", MB_OK);
	return 0;
}
```

也没干什么,先触发一个异常

如果条件允许的话,就开始下面的自解密操作



然后看一下父进程代码

```c
	// 创建调试进程
	GetModuleFileName(GetModuleHandle(NULL),szPath,MAX_PATH);

	if (!CreateProcess(
		NULL, //应用程序的字符串名字
		szPath,//对应的命令行参数
		NULL, //创建的进程的进程句柄是否可以继承有关
		NULL, //创建的进程的线程句柄是否可以继承有关
		FALSE,//创建子进程的时候,是否[需要]继承父进程的句柄表中能够继承的部分
		DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS,// 调试进程参数
		NULL, //环境配置
		NULL,//如果为NULL,那么子进程的argv[0]将会是父进程的地址
		&si,//exe以怎样的方式出现
		&pi))//会返回 进程句柄 ,主线程句柄  ,进程ID   ,线程ID
	{
		printf("CreateProcess() failed! [%d]\n", GetLastError());
		return;
	}
```

父进程是先运行的

然后创建并调试子进程

接着就是一个消息循环

```c
	// 调试器的循环处理
	while (TRUE)
	{
		ZeroMemory(&de, sizeof(DEBUG_EVENT));//初始化为0
		if (!WaitForDebugEvent(
			//获取调试事件,一个阻塞函数
			&de,
			INFINITE))
		{
			printf("WaitForDebugEvent() failed! [%d]\n", GetLastError());
			break;
		}

		if (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)//判断debug事件是不是异常发生了
		{
			dwExcpAddr = (DWORD)de.u.Exception.ExceptionRecord.ExceptionAddress;//异常发生的地址
			dwExcpCode = de.u.Exception.ExceptionRecord.ExceptionCode;//异常类型

			if (dwExcpCode == EXCEPTION_ACCESS_VIOLATION) //故意引发这个异常
			{
				//故意把代码加密,然后引发异常,然后解密
				if (dwExcpAddr == EXCP_ADDR_1)//异常的预期地址,可能我们还需要设置其它的地址,但是是相同的异常
				{
					//修改EIP
					ctx.ContextFlags = CONTEXT_FULL;
					GetThreadContext(pi.hThread, &ctx);
					ctx.Eip += 6;
					SetThreadContext(pi.hThread, &ctx);
				}
			}
		}
		else if (de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
		{
			printf("son exit()\n");
			break;
		}
		ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_CONTINUE); // 异常处理完毕，继续执行子进程
	}
}
```

每次循环都是等待有消息,才会进行下一个动作



下面这个是等待并初始化调试消息

```c
		if (!WaitForDebugEvent(
			//获取调试事件,一个阻塞函数
			&de,
			INFINITE))
		{
			printf("WaitForDebugEvent() failed! [%d]\n", GetLastError());
			break;
		}
```

然后是一个消息类型的判断

```c
		if (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)//判断debug事件是不是异常发生了
		{
			dwExcpAddr = (DWORD)de.u.Exception.ExceptionRecord.ExceptionAddress;//异常发生的地址
			dwExcpCode = de.u.Exception.ExceptionRecord.ExceptionCode;//异常类型

			if (dwExcpCode == EXCEPTION_ACCESS_VIOLATION) //故意引发这个异常
			{
				//故意把代码加密,然后引发异常,然后解密
				if (dwExcpAddr == EXCP_ADDR_1)//异常的预期地址,可能我们还需要设置其它的地址,但是是相同的异常
				{
					//修改EIP
					ctx.ContextFlags = CONTEXT_FULL;
					GetThreadContext(pi.hThread, &ctx);
					ctx.Eip += 6;
					SetThreadContext(pi.hThread, &ctx);
				}
			}
		}
		else if (de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
		{
			printf("son exit()\n");
			break;
		}
```

其中

EXIT_PROCESS_DEBUG_EVENT是退出消息

EXCEPTION_DEBUG_EVENT是调试消息

在调试消息中, EXCEPTION_ACCESS_VIOLATION是访问异常



然后具体的异常修复是下面这个代码,其实就是修改了一下eip

```c
					//修改EIP
					ctx.ContextFlags = CONTEXT_FULL;
					GetThreadContext(pi.hThread, &ctx);//获取
					ctx.Eip += 6;
					SetThreadContext(pi.hThread, &ctx);//写入
```





# 防附加调试

一个调试器要附加调试一个exe

期间,目标exe会调用API: ntdll!DbgUiRemoteBreakin

一般情况下我们是不会调用这个东西的,所以随便hook

于是我们就把它hook成一个exitprocess

当调试器附加的时候,exe调用ntdll!DbgUiRemoteBreakin就会自动退出啦



```c
#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<stdio.h>

int main(int argc, char* argv[])
{
	BYTE	bBuffer[0x10] = { 0 };
	DWORD	dwBreakAddress;					//DbgUiRemoteBreakin函数的地址
	DWORD	dwOldProtect;
	DWORD	dwNum;
	char flag1[] = "flag{xxxxxxxx}";
	char flag2[16];
	dwBreakAddress = (DWORD)GetProcAddress(LoadLibrary("ntdll.dll"), "DbgUiRemoteBreakin");
	bBuffer[0] = 0xE9;												//jmp指令字节码
	*((DWORD*)(bBuffer + 1)) = (DWORD)ExitProcess - (dwBreakAddress + 5);	//ExitProcess函数偏移地址

	VirtualProtect((LPVOID)dwBreakAddress, 0x10, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)dwBreakAddress, bBuffer, 5, &dwNum);
	printf("%x %d\n", dwBreakAddress, dwNum);
	for (int i = 0; i < 5; i++)
	{
		printf("%X", bBuffer[i]);
	}
	puts("");
	VirtualProtect((LPVOID)dwBreakAddress, 0x10, dwOldProtect, &dwOldProtect);

	//此死循环是为了检测
	printf("PLZ flag:");
	scanf("%s", flag2);
	if (!strcmp(flag1, flag2))
	{
		printf("You Got it\n");
	}
	else
	{
		printf("Wrong input\n");
	}
	system("pause");
	return 0;
}
```



# 引申出来的反调试

至少有三个函数可以用来作为调试器附加到一个正在运行的进程上：

-   kernel32!DebugActiveProcess()
-   ntdll!DbgUiDebugActiveProcess()
-   ntdll!NtDebugActiveProcess()

由于一次只能将一个调试器附加到一个进程上，附加到进程上的失败可能表明有另一个调试器存在
