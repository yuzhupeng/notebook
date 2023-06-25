#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

DWORD is_x86 = 0;//进程架构
LPVOID lp_orgWriteFile = NULL;
CREATE_PROCESS_DEBUG_INFO g_cpdi;
BYTE int3 = 0xCC, asm_old = 0;

//C:\Windows\System32\kernel32.dll  x64的dll
//C:\Windows\SysWOW64\kernel32.dll  x86的dll



/*
* 函数名: write0xCC
* 函数参数:
*  - 参数1: 不知道具体含义
* 函数功能: hook writefile的API,方便后面触发异常
*/
BOOL write0xCC(LPDEBUG_EVENT pde)
{
    //下面这2个地址也可以在其它进程获取,而不是写死
    if (is_x86)
    {
        lp_orgWriteFile = (LPVOID)0x77533C50;//API jmp write file
        //77533C50
    }
    else
    {
        //00007FFFA7F25750 
        lp_orgWriteFile = (LPVOID)0x00007FFFA7F25750;//API jmp write file
    }

    // API Hook - WriteFile()
    memcpy(&g_cpdi, &pde->u.CreateProcessInfo, sizeof(CREATE_PROCESS_DEBUG_INFO));
    ReadProcessMemory(g_cpdi.hProcess, lp_orgWriteFile, &asm_old, 1, NULL);   //把原始的字节保存到asm_old
    WriteProcessMemory(g_cpdi.hProcess, lp_orgWriteFile, &int3, 1, NULL);    //往对应的地址那里写入0xCC

    return TRUE;
}
/*
* 函数名:  handler_0xCC
* 函数参数:
*  - 参数1: 不知道具体含义
* 功能:
*   处理int3的异常事件,也就是目标进程发生writefile的时候,本进程该怎么处理
*/
BOOL handler_0xCC(LPDEBUG_EVENT pde)
{
    //分别对应了x86和x64的上下文
    CONTEXT ctx = { 0 };
    PBYTE lpBuffer = NULL;
    DWORD64 dwNumOfBytesToWrite, dwAddrOfBuffer;
    int i;
    PEXCEPTION_RECORD per = &pde->u.Exception.ExceptionRecord;

    // BreakPoint exception (INT 3)
    if (EXCEPTION_BREAKPOINT == per->ExceptionCode || per->ExceptionCode == 0x4000001F)//如果异常事件是0xCC
    {
        //STATUS_WX86_BREAKPOINT 0x4000001F x86的int3
        //EXCEPTION_BREAKPOINT  0x80000003 x64的int3
        if (lp_orgWriteFile == per->ExceptionAddress)//如果发生异常的地方是writefile
        {
            // #1. Unhook ,因为后面别人还要正常执行
            WriteProcessMemory(g_cpdi.hProcess, lp_orgWriteFile, &asm_old, sizeof(BYTE), NULL); //脱去钩子

            if (is_x86)
            {
                // #2. Thread Context 获取线程上下文
                ctx.ContextFlags = CONTEXT_CONTROL;//包括控制寄存器 (RIP, RSP, RFLAGS 等) 的上下文信息。
                GetThreadContext(g_cpdi.hThread, (LPCONTEXT)&ctx);

                // #3. WriteFile() 获取函数的参数
                //   param 2 : ESP + 0x8
                //   param 3 : ESP + 0xC
                ReadProcessMemory(g_cpdi.hProcess, (LPCVOID)(ctx.Rsp + 8), &dwAddrOfBuffer, sizeof(DWORD), NULL);
                ReadProcessMemory(g_cpdi.hProcess, (LPCVOID)(ctx.Rsp + 12), &dwNumOfBytesToWrite, sizeof(DWORD), NULL);
            }
            else
            {
                // #2. Thread Context 获取线程上下文
                ctx.ContextFlags = CONTEXT_INTEGER| CONTEXT_CONTROL; //包括整数寄存器 (RAX, RBX, RCX, RDX 等) 的上下文信息。
                GetThreadContext(g_cpdi.hThread, (LPCONTEXT)&ctx);

                dwAddrOfBuffer = ctx.Rdx;//rdx ; lpBuffe
                dwNumOfBytesToWrite = ctx.R8 ;//r8d nNumberOfBytesToWrite
            }
            // #4. 分配临时缓冲区
            lpBuffer = (PBYTE)malloc(dwNumOfBytesToWrite + 1);
            memset(lpBuffer, 0, dwNumOfBytesToWrite + 1);

            // #5. WriteFile() 复制记事本内容到lp_Buffer
            ReadProcessMemory(g_cpdi.hProcess, (LPVOID)dwAddrOfBuffer, lpBuffer, dwNumOfBytesToWrite, NULL);
            printf("org: %s\n", lpBuffer);

            // #6. 小写>大写
            for (i = 0; i < dwNumOfBytesToWrite; i++)
            {
                if (0x61 <= lpBuffer[i] && lpBuffer[i] <= 0x7A)//如果是小写字母
                {
                    lpBuffer[i] &= 0b11011111;//就都变大写
                }
            }

            printf("convt: %s\n", lpBuffer);

            // #7. 把转化后的数据写入到notepad的缓冲区
            WriteProcessMemory(g_cpdi.hProcess, (LPVOID)dwAddrOfBuffer, lpBuffer, dwNumOfBytesToWrite, NULL);

            // #8. 释放
            free(lpBuffer);

            if (is_x86)
            {
                // #9. Thread Contex 把线程上下文的ip地址设置为lp_orgWriteFile
                ctx.Rip = (DWORD)lp_orgWriteFile;
                SetThreadContext(g_cpdi.hThread, (LPCONTEXT)&ctx);
            }
            else
            {
                // #9. Thread Contex 把线程上下文的ip地址设置为lp_orgWriteFile
                ctx.Rip = lp_orgWriteFile;
                SetThreadContext(g_cpdi.hThread, (LPCONTEXT)&ctx);

            }

            // #10. Debuggee 继续运行被调试者
            ContinueDebugEvent(pde->dwProcessId, pde->dwThreadId, DBG_CONTINUE);
            Sleep(1000);//让其目标线程飞起来,而不知是执行下面的写入0xCC,然后目标线程又被挂起
            // #11. API Hook 再次先=下断点,挂钩
            WriteProcessMemory(g_cpdi.hProcess, lp_orgWriteFile, &int3, sizeof(BYTE), NULL);

            return TRUE;
        }
    }

    return FALSE;
}
/*
* 名字:debugWhile
* 参数:无参数
* 作用: 不断的处理被调试进程的事件
*/
void debugWhile()
{
    DEBUG_EVENT de;//调试事件
    DWORD dwContinueStatus;


    //等待被调试的notepad发生一个事件,然后把信号发过来
    while (WaitForDebugEvent(&de, INFINITE))//不断的去进行一个调试器的消息循环
    {
        dwContinueStatus = DBG_CONTINUE;//被调试者选择 继续执行


        if (CREATE_PROCESS_DEBUG_EVENT == de.dwDebugEventCode)//调试进程生成或者附加事件,只会发生一次
        {
            write0xCC(&de);//对Writefile函数写入0xcc
        }
        else if (EXCEPTION_DEBUG_EVENT == de.dwDebugEventCode)//被调试的notepad发生了异常事件,通常是0xCC
        {
            if (handler_0xCC(&de))//除以0xcc的异常
            {
                continue;
            }
        }
        // 被调试进程终止
        else if (EXIT_PROCESS_DEBUG_EVENT == de.dwDebugEventCode)
        {
            // 被调试者已经终止,调试器也后续的发生终止break
            break;
        }
        // 调速器继续然被调试的程序运行
        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus);//调速器继续运行
    }
}
DWORD FindProcessID(TCHAR* szProcessName)//
{
    DWORD dwPID = -1;
    // SYSTEM_INFO systemInfo;//用于判断进程是x86还是x64的
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;

    // Get the snapshot of the system
    pe.dwSize = sizeof(PROCESSENTRY32);
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

    // find process
    Process32First(hSnapShot, &pe);
    do
    {
#ifdef UNICODE
        if (!wcscmp(szProcessName, pe.szExeFile))

#else
        if (!strcmp(szProcessName, pe.szExeFile))

#endif
        {
            dwPID = pe.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapShot, &pe));

    CloseHandle(hSnapShot);

    return dwPID;
}
int main(int argc, char* argv[])
{
    DWORD dwPID = 0;
    HANDLE hProcess;
    if (0)
    {
        if (argc != 2)
        {
            printf("\n USAGE  : %s <pid>\n", argv[0]);
            return 1;
        }
        // code injection
        dwPID = (DWORD)atol(argv[1]);
    }
    else
    {
        dwPID = FindProcessID(TEXT("notepad.exe"));
    }

    if (dwPID != -1)
    {
        //获取目标进程架构 因为hook x86和x64的不一样
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
        IsWow64Process(hProcess, &is_x86);
        CloseHandle(hProcess);

        if (!DebugActiveProcess(dwPID))//让目标进程被我调试
        {
            printf("DebugActiveProcess(%d) failed!!!\nError Code = %d\n", dwPID, GetLastError());
            return 1;
        }
        debugWhile();//进入一个消息循环
    }
    return 0;
}
