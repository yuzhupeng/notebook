





```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 打开文件
    HANDLE hFile = CreateFile(
        L"C:\\path\\to\\file.txt",  // 文件路径，根据实际情况进行修改
        GENERIC_READ,               // 访问模式，只读
        0,                          // 共享模式，不共享
        NULL,                       // 安全特性，使用默认值
        OPEN_EXISTING,              // 打开已存在的文件
        FILE_ATTRIBUTE_NORMAL,      // 文件属性，正常
        NULL                        // 模板文件句柄，不使用
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("无法打开文件\n");
        return 1;
    }

    // 获取文件大小
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        printf("无法获取文件大小\n");
        CloseHandle(hFile);
        return 1;
    }

    // 分配内存
    LPVOID pMemory = VirtualAlloc(
        NULL,                   // 指定分配的地址，在此使用 NULL 表示由系统选择
        fileSize,               // 分配的内存大小，与文件大小一致
        MEM_COMMIT,             // 内存分配类型，表示提交内存
        PAGE_READWRITE          // 内存页面属性，读写权限
    );
    if (pMemory == NULL) {
        printf("无法分配内存\n");
        CloseHandle(hFile);
        return 1;
    }

    // 读取文件内容到内存
    DWORD bytesRead;
    if (!ReadFile(
        hFile,                  // 文件句柄
        pMemory,                // 缓冲区地址，用于接收文件内容
        fileSize,               // 要读取的字节数，与文件大小一致
        &bytesRead,             // 实际读取的字节数
        NULL                    // 用于传递异步 I/O 句柄，不使用
    )) {
        printf("读取文件失败\n");
        VirtualFree(pMemory, 0, MEM_RELEASE);
        CloseHandle(hFile);
        return 1;
    }

    printf("成功读取 %lu 字节的文件内容到内存\n", bytesRead);

    // 在这里可以使用内存中的文件内容
    // ...

    // 关闭文件和释放内存
    CloseHandle(hFile);
    VirtualFree(pMemory, 0, MEM_RELEASE);

    return 0;
}

```

