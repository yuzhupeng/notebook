



```c

#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hRead, hWrite;
    DWORD bytesRead;
    char buffer[100];

    if (!CreatePipe(&hRead, &hWrite, NULL, 0)) {
        printf("创建管道失败\n");
        return 1;
    }

    // 向管道写入数据
    const char* message = "Hello, pipe!";
    DWORD bytesWritten;
    if (!WriteFile(hWrite, message, strlen(message), &bytesWritten, NULL)) {
        printf("写入管道失败\n");
        CloseHandle(hRead);
        CloseHandle(hWrite);
        return 1;
    }
    CloseHandle(hWrite);

    // 从管道读取数据
    if (!ReadFile(hRead, buffer, sizeof(buffer), &bytesRead, NULL)) {
        printf("读取管道失败\n");
        CloseHandle(hRead);
        return 1;
    }
    CloseHandle(hRead);

    buffer[bytesRead] = '\0';
    printf("从管道读取的数据：%s\n", buffer);

    return 0;
}

```



