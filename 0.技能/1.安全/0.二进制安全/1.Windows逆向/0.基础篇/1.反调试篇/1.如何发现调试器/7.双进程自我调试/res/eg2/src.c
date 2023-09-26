#include <windows.h>
#include <stdio.h>


#define DEF_MUTEX_NAME      "ReverseCore:DebugMe4"


void DoParentProcess();// 父进程函数
void DoChildProcess();// 子进程函数


void main()
{
    HANDLE hMutex = NULL;

    if (!(hMutex = CreateMutex( //互斥体创建
        __in_opt NULL, //安全属性,垃圾,NULL
        __in  FALSE, //互斥信号不属于任何人
        __in_opt  DEF_MUTEX_NAME //互斥的字符串名字,随便取
    )))
    {
        printf("CreateMutex() failed! [dec:%d]\n", GetLastError());// 创建mutex失败
        return;
    }

    // 检查mutex，判断子进程和父进程
    if (ERROR_ALREADY_EXISTS != GetLastError())
        DoParentProcess();
    else
        DoChildProcess();
}

void DoChildProcess()
{
    MessageBox(NULL, "子进程", "香Sir", MB_OK);
    int i = 0;
    char flag[] = {
        0x74,0x40,0x57,0x43,
        0x6b,0x62,0x77,0x66,
        0x65,0x6a,0x21,0x76,
        0x21,0x95,0x77,0x9c,
        0x58,0x92,0x5d,0x93,
        0x65,0x8d,0 };
    // 下面是引发异常的汇编,自己可以DIY
    __asm
    {
       xor eax,eax
       mov dword ptr [eax],0
    }
    for (i = 0; i < 22; i++)
    {
        if (i & 1 == 1)
        {
            flag[i] = (flag[i] -0x20)^ 0x10;
        }
        else
        {
            flag[i] = (flag[i] -0x10) ^ 0x20;
        }
        printf("%c", flag[i]);
    }
    MessageBox(NULL, flag, "Good Boy", MB_OK);
}
void DoParentProcess()
{
    CHAR                    szPath[MAX_PATH] = { 0, };
    STARTUPINFO				si = { sizeof(STARTUPINFO), };
    PROCESS_INFORMATION		pi = { 0, };
    DEBUG_EVENT             de = { 0, };
    CONTEXT                 ctx = { 0, };
    BYTE                    pBuf[256] = { 0, };
    DWORD                   dwExcpAddr = 0, dwExcpCode = 0;
    const DWORD             DECODING_SIZE = 100;
    const DWORD             DECODING_KEY = 0x90;
    const DWORD             EXCP_ADDR_1 = 0x00411887;

    MessageBox(NULL, "父进程", "香Sir", MB_OK);
    // 创建调试进程
    GetModuleFileName(
        GetModuleHandle(NULL),
        szPath,
        MAX_PATH);

    if (!CreateProcess(
        __in_opt  NULL, //应用程序的字符串名字
        __inout_opt szPath,//对应的命令行参数
        __in_opt NULL, //创建的进程的进程句柄是否可以继承有关
        __in_opt NULL, //创建的进程的线程句柄是否可以继承有关
        __in FALSE,//创建子进程的时候,是否[需要]继承父进程的句柄表中能够继承的部分
        __in DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS,// 调试进程参数
        __in_opt NULL, //环境配置
        __in_opt NULL,//如果为NULL,那么子进程的argv[0]将会是父进程的地址
        __in  &si,//exe以怎样的方式出现
        __out &pi))//会返回 进程句柄	,主线程句柄	,进程ID   ,线程ID
    {
        printf("CreateProcess() failed! [%d]\n", GetLastError());
        return;
    }

    // 调试器的循环处理
    while (TRUE)
    {
        ZeroMemory(&de, sizeof(DEBUG_EVENT));//初始化为0

        if (!WaitForDebugEvent(//获取调试事件,一个阻塞函数
            __out &de,
            __in INFINITE))
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
                    // decoding
                    ReadProcessMemory( // 读子进程内存空间
                        pi.hProcess,
                        (LPCVOID)(dwExcpAddr + 6),//异常代码处占用6字节
                        pBuf,
                        DECODING_SIZE,
                        NULL);

                    for (DWORD i = 0; i < DECODING_SIZE; i++) // 进行异或解码
                        pBuf[i] ^= DECODING_KEY;

                    WriteProcessMemory(// 写回子进程内存空间
                        pi.hProcess,
                        (LPVOID)(dwExcpAddr + 6),
                        pBuf,
                        DECODING_SIZE,
                        NULL);

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
            printf("子进程发出退出的信号\n");
            break;
        }

        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_CONTINUE); // 异常处理完毕，继续执行子进程
    }
}
 