# 基础

大名: TopLevel Exception Filter

小名: UEF

返回值

```c
EXCEPTION_EXECUTE_HANDLER(1): 		//已经处理 程序直接安静的退出
EXCEPTION_CONTINUE_SEARCH(0): 		//丢给下一个函数(已经没有函数了)	而卡死退出
EXCEPTION_CONTINUE_EXECUTION(-1): 	//继续执行异常处 如果有VCH就程引出VCH,而不是继续执行异常处
```

安装

SetUnhandledExceptionFilter(UEF);//调试的时候不会执行,非调试的时候会执行

是整个异常处理的最后一环,也可以说不是最后一环

UnhandledExceptionFilter内部调用NtQueryInformationProcess以判断是否正在调试进程

若进程正常运行,会调用UEF,看返回值调用VCH

若进程处于调试,UEF不会被调用,直接运行VCH

返回 EXCEPTION_CONTINUE_SEARC时，

会查注册表，检查是否存在实时调试器。

注册表路径：KLM\software\microsoft\windows nt\currentvsrsion\aedebug。

如果Auto==1，Debugger!=NULL则根据Debugger中指示的参数启动实时调试器，

让调试器处理该异常。

如果不存在顶层异常且进程没被调试，也会检查并启动实时调试器

```c
#include <windows.h>
#include <stdio.h>
LONG WINAPI TopLevelExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
    printf("ExceptionCode: %X\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
    ExceptionInfo->ContextRecord->Eax = 1;
    ExceptionInfo->ContextRecord->Ecx = 1;
    return EXCEPTION_CONTINUE_EXECUTION;
    //return EXCEPTION_EXECUTE_HANDLER;
    //return EXCEPTION_CONTINUE_SEARCH;
}

int main()
{
    int number = 0;

    // 通过一个函数可以直接的安装 UEF
    SetUnhandledExceptionFilter(TopLevelExceptionFilter);

    __try
    {
        number /= 0;
        printf("Find Me?\n");
    }
    // 异常一旦被 SEH 处理，就不会再传递给 UEF
    __except (EXCEPTION_CONTINUE_SEARCH)
    {
        printf("不会被执行\n");//一定不会执行呀
    }

    printf("number = %d\n", number);

    system("pause");
    return 0;
}
```