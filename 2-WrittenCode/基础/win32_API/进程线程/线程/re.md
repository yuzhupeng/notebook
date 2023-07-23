



```c
HANDLE CreateRemoteThread(
  [in]  HANDLE                 hProcess,
  [in]  LPSECURITY_ATTRIBUTES  lpThreadAttributes,
  [in]  SIZE_T                 dwStackSize,
  [in]  LPTHREAD_START_ROUTINE lpStartAddress,
  [in]  LPVOID                 lpParameter,
  [in]  DWORD                  dwCreationFlags,
  [out] LPDWORD                lpThreadId
);
```







创建线程



```c
#include <Windows.h>
#include <stdio.h>

// 线程函数
DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
    int threadId = *((int*)lpParam);
    printf("线程 %d 正在执行...\n", threadId);

    // 在这里执行你希望线程完成的任务

    printf("线程 %d 执行完成.\n", threadId);
    return 0;
}

int main() {
    HANDLE hThread;
    DWORD threadId;
    int myData = 123; // 传递给线程的数据

    // 创建线程
    hThread = CreateThread(NULL, 0, MyThreadFunction, &myData, 0, &threadId);
    if (hThread == NULL) {
        printf("无法创建线程. 错误码: %d\n", GetLastError());
        return 1;
    }

    // 等待线程结束
    WaitForSingleObject(hThread, INFINITE);

    // 关闭线程句柄
    CloseHandle(hThread);

    return 0;
}

```

