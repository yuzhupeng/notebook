

很多API可以自己去Load,而不是直接调用,这样可能会隐蔽一点点

有一款OD的插件StrongOD, 它是基于内核的,利用到了Hook相关技术,可以绕过很多的API 反调试



# IsDebuggerPresent

```c
#include<Windows.h>
BOOL IsDebuggerPresent();
```

如果当前进程在调试器上下文中运行，则返回值为1。

如果当前进程未在调试器的上下文中运行，则返回值为0。

```c
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
 
int main()
{
	if (IsDebuggerPresent())
	{
		printf("you are dbg me!");
	}
	else
	{
		printf("all right");
	}

	return 0;
}
```



# CheckRemoteDebuggerPresent

`kernel32`的`CheckRemoteDebuggerPresent`函数用于检测指定进程是否正在被调试.

CheckRemoteDebuggerPresent和IsDebuggerPresent类似



```c
#define _CRT_SECURE_NO_WARNINGS

# include<Windows.h>
# include<stdio.h>
BOOL CheckDebug()
{
	BOOL isDebugged;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugged);
	return isDebugged;
}
int main(int argc, char* argv[])
{
	if (CheckDebug())
		printf("Find You \n");
	else
		printf("where are you\n");
	return 0;
}
```







# NtSetInformationThread

应该和线程监测有关

```c
typedef enum _THREADINFOCLASS {
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger,  //这个用来反调试可能比较常见
	ThreadBreakOnTermination,
	MaxThreadInfoClass
} THREADINFOCLASS;
```

```c
__kernel_entry NTSYSCALLAPI NTSTATUS NtSetInformationThread(
  [in] HANDLE          ThreadHandle, //当前线程对象的句柄
  [in] THREADINFOCLASS ThreadInformationClass, //枚举结构体的成员
  [in] PVOID           ThreadInformation,
  [in] ULONG           ThreadInformationLength
);
```

如果正常运行,他会分离,但是会终止

如果是调试,他会分离和终止

里面的枚类型都是自定义的,

encode是反调试

decode是解除反调试,只要传入的不是0x11,应该都行

eg1:

```c
#include <Windows.h>
#include <stdio.h>

// 反调试函数
void HideFromDebugger() 
{
	DWORD tmp1;
	DWORD tmp2;
	HMODULE hNtDll = LoadLibraryA("ntdll.dll");
	if (hNtDll)
	{
		tmp1 = (DWORD)GetProcAddress(hNtDll, "NtSetInformationThread");
		tmp2 = GetCurrentThread();
		_asm {
			push 0
			push 0
			push 0x11
			push tmp2
			call tmp1
		}
	}

	return 0;
}

// 线程函数
void testFunc()
{
	HideFromDebugger();
	while (1)
	{
		printf("Hell0\n");
		Sleep(1000);
	}
}
int main()
{
	testFunc();
	return 0;
}
```

eg2:



但是我们是可以通过汇编调用的

ps: 现在不会,说是要进入内核调用

下面是一个错误的代码



关于Hook,下面是一个无法运行的代码



# NtCreateThreadEx



Windows从Vista开始引入了`NtCreateThreadEx`函数，函数原型如下所示

```
NTSTATUS NTAPI NtCreateThreadEx (
    _Out_    PHANDLE              ThreadHandle,
    _In_     ACCESS_MASK          DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES   ObjectAttributes,
    _In_     HANDLE               ProcessHandle,
    _In_     PVOID                StartRoutine,
    _In_opt_ PVOID                Argument,
    _In_     ULONG                CreateFlags,
    _In_opt_ ULONG_PTR            ZeroBits,
    _In_opt_ SIZE_T               StackSize,
    _In_opt_ SIZE_T               MaximumStackSize,
    _In_opt_ PVOID                AttributeList
);
```

其中最有趣的参数是`CreateFlags`，该参数可以使用如下标志

```
#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET 0x00000020
#define THREAD_CREATE_FLAGS_INITIAL_THREAD 0x00000080
```

如果新线程设置了`THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER`标志，那么在创建时就可以向调试器隐藏该线程信息，

这与`NtSetInformationThread`函数设置的`ThreadHideFromDebugger`相同。

负责安全任务的代码可以在设置`THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER`标志的线程中执行。

传入值为4的时候，貌似OD里对这个线程里的代码下断会飞，似乎是hidefromdbgger的功能。



eg

```c
 #include <Windows.h>
typedef NTSTATUS(NTAPI* pfnNtCreateThreadEx) (
	_Out_    PHANDLE              ThreadHandle,
	_In_     ACCESS_MASK          DesiredAccess,
	_In_opt_ PVOID		  ObjectAttributes,		//POBJECT_ATTRIBUTES
	_In_     HANDLE               ProcessHandle,
	_In_     PVOID                StartRoutine,
	_In_opt_ PVOID                Argument,
	_In_     ULONG                CreateFlags,
	_In_opt_ ULONG_PTR            ZeroBits,
	_In_opt_ SIZE_T               StackSize,
	_In_opt_ SIZE_T               MaximumStackSize,
	_In_opt_ PVOID                AttributeList
	);
//线程回调函数
void ThreadProc()
{
	MessageBox(NULL, TEXT("我是一个新线程！"), NULL, MB_OK);
}

int main(int argc, char* argv[])
{
	HANDLE hThread;
	pfnNtCreateThreadEx NtCreateThreadEx;
	NtCreateThreadEx = (pfnNtCreateThreadEx)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtCreateThreadEx");
	NtCreateThreadEx(
		&hThread,
		0x1FFFFF,
		NULL,
		GetCurrentProcess(),
		(LPTHREAD_START_ROUTINE)ThreadProc,
		NULL,
		0x00000004,					//直接执行并且对调试器隐藏THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER
		NULL,
		NULL,
		NULL,
		NULL
	);
	WaitForSingleObject(hThread, INFINITE);
	int n = GetLastError();
	return 0;
}



```



如果我们对远程线程下一个断点的话

就会导致调试器分离,崩溃





# NtQueryInformationProcess 查询进程

```c
ProcessDebugPort            // 0x7
ProcessDebugFlags            // 0x1F
ProcessDebugObjectHandle    // 0x1E
ProcessBasicInformation        // 0x0
```



根据一些进程的端口值去判断

```c
#include <stdio.h>
#include <windows.h>

enum encode
{
	Ask_Pro_Dbg_Port = 7,//看这里,非调试状态下他为0,调试状态下为0xFFFFFFFF
	Ask_Proc_Dbg_Obj_Handle = 30,//看这里 正常运行时为NULL
	Ask_Proc_Dbg_Flags = 31,//看这里 正常运行时为1,调试时为0
};
typedef NTSTATUS(WINAPI* NT_Query_Info_Proc)
(
	HANDLE ProcessHandle,
	enum encode ask_info,
	PVOID proc_info_p_out,
	ULONG ProcessInformationLength,
	PULONG ReturnLength
	);
void Check_NtQueryInformationProcess()
{
	HMODULE tmp = NULL;
	HANDLE tmp2 = NULL;
	NT_Query_Info_Proc ptr_func = NULL;

	DWORD check1 = 0;// Ask_Pro_Dbg_Port (0x7)
	DWORD check2 = NULL;// Ask_Proc_Dbg_Obj_Handle (0x1E)
	DWORD check3 = TRUE;// Ask_Proc_Dbg_Flags (0x1F)

	tmp = GetModuleHandle("ntdll.dll"); //难道LoadLibiary和GetModuleHandle的功能是一样的??
	if (tmp == NULL)
		exit(-1);
	ptr_func = (NT_Query_Info_Proc)GetProcAddress(tmp, "NtQueryInformationProcess");

	tmp2 = GetCurrentProcess();
	ptr_func(tmp2, Ask_Pro_Dbg_Port, &check1, sizeof(check1), NULL);
	ptr_func(tmp2, Ask_Proc_Dbg_Obj_Handle, &check2, sizeof(check2), NULL);
	ptr_func(tmp2, Ask_Proc_Dbg_Flags, &check3, sizeof(check3), NULL);

	if ((check1 != 0) && (check2 != NULL) && (check3 != 1))
	{
		MessageBox(0, "Find YOu\n", "redqx", 0);
	}
	else
	{
		MessageBox(0, "Where are You\n", "redqx", 0);
	}
}

int main()
{
	Check_NtQueryInformationProcess();
	return 0;
}
```

 

# 未掌握



## NtQueryObject

主要是一些结构体里面存储着某些东西,然后去遍历那个信息

NtQueryObject_pf(NULL, get_all, &Size_ul, sizeof(Size_ul), &Size_ul);

比如这里的

```c
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

typedef enum _Obj_Info_Class_enum
{
    ObjectBasicInformation,
    ObjectNameInformation,
    ObjectTypeInformation,
    get_all,//只会用到这一个参数
    ObjectHandleInformation
} just_enum;
typedef struct _LSA_Unicode_Str_struct
{
    USHORT len;
    USHORT Max_Len;
    PWSTR Buf;
}Uncode_Str_struct;

#pragma pack(1) //这里的意思是下面结构体的对齐方式是按照一个字节,这样的话数据分布会比较的紧凑,节约空间
typedef struct _Obj_Type_info_struct
{
    Uncode_Str_struct TypeName;
    ULONG Handles_len;
    ULONG Objects_en;
}Obj_Type_info_struct, * Obj_Type_info_pstruct;

typedef struct _obj_All_info_struct
{
    ULONG                   count;
    Obj_Type_info_struct member_struct[1];
}*obj_info_pstruct;
#pragma pack()

typedef NTSTATUS(WINAPI* NT_Query_Obj_pf)
(
    HANDLE Handle,
    just_enum ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
    );

void Check_NtQueryObj()
{

    obj_info_pstruct Buf_pstruct = NULL;
    void* Buf = NULL;
    ULONG Size_ul = 0;
    BOOL Debugging = FALSE;//默认没有开启
    HMODULE tmp = NULL;
    //输入参数不同,函数的结果大为不同

    tmp = GetModuleHandle("ntdll.dll");
    if (tmp == NULL)
        exit(-1);
    //获取导出函数的地址
    NT_Query_Obj_pf NtQueryObject_pf = (NT_Query_Obj_pf)GetProcAddress(tmp, "NtQueryObject");

    // 输入链表的的size,这种参数的形式是为了获取size的大小
    NtQueryObject_pf(NULL, get_all, &Size_ul, sizeof(Size_ul), &Size_ul);

    // 分配链表的缓冲,大小为刚才利用函数pf_NtQueryObject而获取的
    Buf = VirtualAlloc(NULL, Size_ul, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (Buf == NULL)
        exit(-1);

    // 输入实际的链表,所有的信息都返回在了第三个参数
    NtQueryObject_pf((HANDLE)0xFFFFFFFF, get_all, Buf, Size_ul, NULL);//这和上面那个参数不一样

    Buf_pstruct = (obj_info_pstruct)Buf;//这个变量的类型是 _Obj_Info_Class_enum 的第4个成员

    UCHAR* member_puc = (UCHAR*)Buf_pstruct->member_struct;
    Obj_Type_info_pstruct Obj_Type_Info_tmp_pstruct = NULL;

    for (UINT i = 0; i < Buf_pstruct->count; i++)
    {
        Obj_Type_Info_tmp_pstruct = (Obj_Type_info_pstruct)member_puc;
        if (strcmp("DebugObject", Obj_Type_Info_tmp_pstruct->TypeName.Buf) == 0)
        {
            Debugging = (Obj_Type_Info_tmp_pstruct->Objects_en > 0) ? TRUE : FALSE;
            break;
        }
        //计算下一个结构体
        member_puc = (UCHAR*)Obj_Type_Info_tmp_pstruct->TypeName.Buf;
        member_puc += Obj_Type_Info_tmp_pstruct->TypeName.len;
        member_puc = (UCHAR*)(((ULONG)member_puc & 0xFFFFFFFC) + sizeof(ULONG));
    }
    if (Buf)
    {
        VirtualFree(Buf, 0, MEM_RELEASE);
        MessageBox(0, "Where are You\n", "Xsir", 0);
    }
    if (Debugging)
    {
        MessageBox(0, "Find YOu\n", "Xsir", 0);
    }
}

int main()
{
    Check_NtQueryObj();
    return 0;
}
```

## CheckGlobalFlags(失败)

可执行文件既包括IMAGE_LOAD_CONFIG_DIRECTORY结构，也包括系统加载程序的其他配置参数。

不过在默认情况下，此结构不会内置到可执行文件中，需要使用补丁添加。此结构具有GlobalFlagsClear字段，

对PEB结构中要重置的NtGlobalFlag字段进行了标识。如果最初没有对该结构或GlobalFlagsClear = 0创建可执行文件，

那么在磁盘或内存中，该字段就具有非零值，隐藏的调试器就会正常运行。

下面就是检查运行进程的内存和磁盘上的GlobalFlagsClear字段的代码，这是一种流行的反调试技术：

作者写失败了? 我表示不懂原理,不会修改,反正怎么调试都不报错

```c
#define _CRT_SECURE_NO_WARNINGS

# include<Windows.h>
# include<stdio.h>

PIMAGE_NT_HEADERS GetImageNtHeaders(PBYTE pImageBase)
{
    PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
    return (PIMAGE_NT_HEADERS)(pImageBase + pImageDosHeader->e_lfanew);
}
PIMAGE_SECTION_HEADER FindRDataSection(PBYTE pImageBase)
{
    char rdata[] = ".rdata";
    PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
    PIMAGE_SECTION_HEADER pImageSectionHeader = IMAGE_FIRST_SECTION(pImageNtHeaders);
    int n = 0;
    for (; n < pImageNtHeaders->FileHeader.NumberOfSections; ++n)
    {
        if (!strcmp(rdata ,(char*)pImageSectionHeader[n].Name))
        {
            break;
        }
    }
    return &pImageSectionHeader[n];
}
void CheckGlobalFlagsClearInProcess()
{
    PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
    PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
    PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pImageBase
        + pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress);
    if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
    {
        printf("Stop debugging program!\n");
        exit(-1);
    }
}
void CheckGlobalFlagsClearInFile()
{
    HANDLE hExecutable = INVALID_HANDLE_VALUE;
    HANDLE hExecutableMapping = NULL;
    PBYTE pMappedImageBase = NULL;
    __try
    {
        PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
        PIMAGE_SECTION_HEADER pImageSectionHeader = FindRDataSection(pImageBase);
        TCHAR pszExecutablePath[MAX_PATH];
        DWORD dwPathLength = GetModuleFileName(NULL, pszExecutablePath, MAX_PATH);
        if (0 == dwPathLength) __leave;
        hExecutable = CreateFile(pszExecutablePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (INVALID_HANDLE_VALUE == hExecutable) __leave;
        hExecutableMapping = CreateFileMapping(hExecutable, NULL, PAGE_READONLY, 0, 0, NULL);
        if (NULL == hExecutableMapping) __leave;
        pMappedImageBase = (PBYTE)MapViewOfFile(hExecutableMapping, FILE_MAP_READ, 0, 0,
            pImageSectionHeader->PointerToRawData + pImageSectionHeader->SizeOfRawData);
        if (NULL == pMappedImageBase) __leave;
        PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pMappedImageBase);
        PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pMappedImageBase
            + (pImageSectionHeader->PointerToRawData
                + (pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress - pImageSectionHeader->VirtualAddress)));
        if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
        {
            printf("Stop debugging program!\n");
            exit(-1);
        }
    }
    __finally
    {
        if (NULL != pMappedImageBase)
            UnmapViewOfFile(pMappedImageBase);
        if (NULL != hExecutableMapping)
            CloseHandle(hExecutableMapping);
        if (INVALID_HANDLE_VALUE != hExecutable)
            CloseHandle(hExecutable);
    }
}
int main()
{
    CheckGlobalFlagsClearInFile();
    CheckGlobalFlagsClearInProcess();
    puts("HelloWorld");
	return 0;
}
```

在此代码中，CheckGlobalFlagsClearInProcess函数会通过加载当前运行

的进程地址查找PIMAGE_LOAD_CONFIG_DIRECTORY结构，

并检查GlobalFlagsClear字段值。如果不是0，那么该进程可能被调试。

CheckGlobalFlagsClearInFile函数也会执行相同的检查，但仅仅针对的是磁盘上的可执行文件





## GetLastError(可能失败)



一个未完成的代码

如果不加异常的话,代码在直接运行的时候会g

相关的类似的还有

```
OutputDebugString("Test for debugger!");
CloseHandle()
CloseWindow()
产生异常，使得错误码改变
但是有时候,异常发生后,程序就直接g了
```





```c
#include <Windows.h>
#include <stdio.h>

BOOL CheckDebug1()
{
    DWORD ret = CloseHandle((HANDLE)0x12345678);
    if (ret != 0 || GetLastError() != ERROR_INVALID_HANDLE)//还可以使用
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CheckDebug2()
{
   // DWORD ret = CloseWindow((HWND)0x9999);//反正函数不会失败,不知道为什么
    DWORD ret = (DWORD)LoadLibrary((char*)0x12345678);//反正函数不会失败,不知道为什么
    if (ret != 0 || GetLastError() != ERROR_INVALID_WINDOW_HANDLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
BOOL CheckDebug3()
{
    char fib[1024] = { 0 };
    DeleteFiber(fib);//这个应该还可以使用,但是得异常处理一下,我猜的
    return (GetLastError() != 0x57);
}
BOOL CheckDebug4()
{
    DWORD errorValue = 12345;
    SetLastError(errorValue);
    OutputDebugString("Test for debugger!");
    if (GetLastError() == errorValue)//好像一直用不了了
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

    if (CheckDebug2())
    {
        printf("Check You");
    }
    else
    {
        printf("where are you\n");
    }
	return 0;
}
```



