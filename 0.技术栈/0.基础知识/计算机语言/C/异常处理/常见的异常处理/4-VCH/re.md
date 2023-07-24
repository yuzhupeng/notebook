# VCH

大名: Vectored Continue Handler

小名: VCH

进程相关

返回值

```c
EXCEPTION_CONTINUE_SEARCH：（0）
EXCEPTION_CONTINUE_EXECUTION：（1）
```

只有 当上一次异常处理返回的是 EXCEPTION_CONTINUE_EXECUTION 才会引出VCH

否则VCH是不会被调用的



VCH注册和移除的方法：

使用AddVectoredContinueHandler函数。

安装 AddVectoredContinueHandler(TRUE, VCH);//VCH自己也有自己的VCH异常处理链表

使用RemoveVectoredContinueHandler函数移除。

Example

```c
#include <windows.h>
#include <stdio.h>

DWORD SEH(PEXCEPTION_POINTERS ExceptionInfo)
{
    printf("SEH: ExceptionCode: %X\n", ExceptionInfo->ExceptionRecord->ExceptionCode);

    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO)
    {
        ExceptionInfo->ContextRecord->Eax =1;
        ExceptionInfo->ContextRecord->Ecx = 1;

        return EXCEPTION_CONTINUE_EXECUTION;
        //return EXCEPTION_EXECUTE_HANDLER;
        //return EXCEPTION_CONTINUE_SEARCH;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI VCH(PEXCEPTION_POINTERS ExceptionInfo)
{
    // VCH 不会对异常进行处理，调用的时机和异常处理的情况有关
    printf("VCH: ExceptionCode: %X\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
    ExceptionInfo->ContextRecord->Eax = 1;
    ExceptionInfo->ContextRecord->Ecx = 1;
    return EXCEPTION_CONTINUE_SEARCH;
}

int main()
{
    int number = 0;

    AddVectoredContinueHandler(TRUE, VCH);

    __try
    {
        number /= 0;
        printf("Find Me?");
    }
    __except (SEH(GetExceptionInformation()))
    {
        printf("SEH: 异常处理器返回\n");
    }

    printf("number = %d\n", number);

    system("pause");
    return 0;
}
```