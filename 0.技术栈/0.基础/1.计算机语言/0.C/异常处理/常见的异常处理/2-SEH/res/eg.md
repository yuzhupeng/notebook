

# 1



```c
#include <stdio.h>
//==================================================
// MYSEH - Matt Pietrek 1997
// Microsoft Systems Journal, January 1997
// FILE: MYSEH.CPP
// 用命令行CL MYSEH.CPP编译
//==================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
DWORD scratch;

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
    printf("ExceptionCode: %X\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0xc0000005)
    {
        ExceptionInfo->ContextRecord->Eax = 1;
        ExceptionInfo->ContextRecord->Ecx = 1;

        return EXCEPTION_CONTINUE_SEARCH;
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
EXCEPTION_DISPOSITION _cdecl _except_handler(struct _EXCEPTION_RECORD* ExceptionRecord,
    void* EstablisherFrame,
    struct _CONTEXT* ContextRecord,
    void* DispatcherContext)
{
    unsigned i;

    printf("%08x\n", ExceptionRecord->ExceptionCode);
    ContextRecord->Eax = (DWORD)&scratch;
    return ExceptionContinueSearch;
    //ExceptionContinueSearch;
    // ExceptionContinueExecution;
    //基于异常表达式的返回值, 基于SEH函数的返回值?

    /*

    typedef enum _EXCEPTION_DISPOSITION
{
    ExceptionContinueExecution,0 <- 1
    ExceptionContinueSearch,1 <- 0
    ExceptionNestedException,2
    ExceptionCollidedUnwind3

} EXCEPTION_DISPOSITION;*/
}
LONG FirstSEHer(PEXCEPTION_POINTERS pExcepInfo)
{

    printf("%08x\n", pExcepInfo->ExceptionRecord->ExceptionCode);
    return EXCEPTION_CONTINUE_SEARCH;
}
LONG SecondSEHer(PEXCEPTION_POINTERS pExcepInfo)
{
    printf("%08x\n", pExcepInfo->ExceptionRecord->ExceptionCode);
    return EXCEPTION_CONTINUE_EXECUTION;

    //return EXCEPTION_EXECUTE_HANDLER;
}
void ExceptTest()
{
    DWORD handler = (DWORD)_except_handler;
    DWORD* lp = 0;
    __asm
    {
        // 创建EXCEPTION_REGISTRATION结构：
        push handler // handler函数的地址
        push FS : [0] // 前一个handler函数的地址
        mov  FS : [0] , ESP // 安装新的EXECEPTION_REGISTRATION结构
    }
    printf("After writing!\n");
    __try
    {
        __try
        {
            __try
            {
                *lp = 0;
                printf("Find Me?\n");
            }
            __finally
            {
                printf("[[SEH][0] finally \n");
            }
        }
        __except (FirstSEHer(GetExceptionInformation()))
        {
            printf("[EH.Exe] [SEH][1] 被俺处理了 SEH1 返回 EXCEPTION_EXECUTE_HANDLER\n");
        }
    }
    __except (SecondSEHer(GetExceptionInformation()))
    {
        printf("[EH.Exe] [SEH][2] 被俺处理了 SEH2 返回 EXCEPTION_EXECUTE_HANDLER\n");
    }
    __asm
    {
        // 移去我们的EXECEPTION_REGISTRATION结构
        mov eax, [ESP]    // 获取前一个结构
        mov FS : [0] , EAX // 安装前一个结构
        add esp, 8       // 将我们的EXECEPTION_REGISTRATION弹出堆栈
    }
}

int main()
{

    AddVectoredExceptionHandler(TRUE, VectoredExceptionHandler);
    ExceptTest();
    return 0;
}
```



# 2



```c
#include <stdio.h>
//==================================================
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
DWORD scratch;

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
    printf("VEH:%08X\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0xc0000005)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
EXCEPTION_DISPOSITION  SEH1(struct _EXCEPTION_RECORD* ExceptionRecord,
    void* EstablisherFrame,
    struct _CONTEXT* ContextRecord,
    void* DispatcherContext)
{
    unsigned i;
    printf("SEH:%08X\n", ExceptionRecord->ExceptionCode);
    ContextRecord->Eax = (DWORD)&scratch;
    return ExceptionContinueExecution;
}
void ExceptTest()
{
    DWORD* lp = 0;
    __asm
    {
        // 创建EXCEPTION_REGISTRATION结构：
        push SEH1 // handler函数的地址
        push FS : [0] // 前一个handler函数的地址
        mov  FS : [0] , ESP // 安装新的EXECEPTION_REGISTRATION结构
    }
    *lp = 0;
    __asm
    {
        // 移去我们的EXECEPTION_REGISTRATION结构
        mov eax, [ESP]    // 获取前一个结构
        mov FS : [0] , EAX // 安装前一个结构
        add esp, 8       // 将我们的EXECEPTION_REGISTRATION弹出堆栈
    }
}

int main()
{
    AddVectoredExceptionHandler(TRUE, VectoredExceptionHandler);
    ExceptTest();
    return 0;
}
```

