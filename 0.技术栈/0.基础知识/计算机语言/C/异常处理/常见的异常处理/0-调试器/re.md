

如果出现异常的程序正在被调试，

则该异常首先交给调试器处理(通过DebugPort导出异常信息)。

调试器拿到这个异常后，需要判断是否要处理该异常，

如果处理该异常返回



`DBG_CONTINUE == EXCEPTION_CONTINUE_EXECUTION` (默认是处理了,并让exe在原EIP处继续执行)

`DBG_EXCEPTION_NOT_HANDLED == EXCEPTION_CONTINUE_SEARCH` (调速器不处理器,把异常交给exe自己的处理函数)



常见的处理

```c


// debug loop
while( TRUE )
{
    ZeroMemory(&de, sizeof(DEBUG_EVENT));

    if( !WaitForDebugEvent(&de, INFINITE) )
    {
        printf("WaitForDebugEvent() failed! [%d]\n", GetLastError());
        break;
    }

    if( de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT )
    {
        //进行你的调速器处理
    }
    else if( de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT )
    {
        break;
    }

    ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_CONTINUE);
}
```