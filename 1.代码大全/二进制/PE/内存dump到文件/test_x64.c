#include <windows.h>
#include <stdio.h>

 
// 获取API函数地址

BYTE* va2fa(BYTE* base,int fileSize)
{  // 获取导出表
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)base;
    PIMAGE_NT_HEADERS64 ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    IMAGE_SECTION_HEADER* secHeader = ntHeaders->FileHeader.SizeOfOptionalHeader + (BYTE*)&ntHeaders->OptionalHeader;
    // 分配内存
    int filsize_new = 0;
    BYTE* pMemory = VirtualAlloc(
        NULL,                   // 指定分配的地址，在此使用 NULL 表示由系统选择
        fileSize,               // 分配的内存大小，与文件大小一致
        MEM_COMMIT,             // 内存分配类型，表示提交内存
        PAGE_READWRITE          // 内存页面属性，读写权限
    );
    BYTE* pMemory2 = pMemory+4;
    filsize_new = ntHeaders->OptionalHeader.SizeOfHeaders;
    memcpy(pMemory2, base, filsize_new);
    pMemory2 = pMemory2 + filsize_new;
    
    int numSection = ntHeaders->FileHeader.NumberOfSections;
    for (int i = 0; i < numSection; i++)
    {
        filsize_new += secHeader->SizeOfRawData;
        memcpy(pMemory2, secHeader->VirtualAddress + base, secHeader->SizeOfRawData);
        secHeader++;
        pMemory2 += secHeader->SizeOfRawData;
    }
    *(DWORD*)pMemory = filsize_new;
    return pMemory;
}
int writeFile(WCHAR* fpath,BYTE* data,int dlen)
{
    // 打开文件
    HANDLE hFile = CreateFileW(fpath,  // 文件路径
        GENERIC_WRITE,              // 访问权限：写入
        0,                          // 共享模式：无
        NULL,                       // 安全属性：默认
        CREATE_ALWAYS,              // 打开方式：总是创建新文件
        FILE_ATTRIBUTE_NORMAL,      // 文件属性：普通
        NULL);                      // 模板文件句柄：无

    if (hFile == INVALID_HANDLE_VALUE)
    {
        // 文件打开失败
        printf("Failed to open the file. Error code: %d\n", GetLastError());
        return 1;
    }

    // 写入数据
    DWORD bytesWritten = 0;
    if (!WriteFile(hFile, data, dlen, &bytesWritten, NULL))
    {
        // 写入数据失败
        printf("Failed to write data to the file. Error code: %d\n", GetLastError());
        CloseHandle(hFile);
        return 1;
    }

    // 关闭文件
    CloseHandle(hFile);

    // 写入完成
    printf("Data written to the file successfully.\n");

    return 0;
}
BYTE* readFile(WCHAR* fpath) {
    void* newFileData;
    // 打开文件
    HANDLE hFile = CreateFileW(
        fpath,  // 文件路径，根据实际情况进行修改
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

    newFileData = va2fa(pMemory, bytesRead);

    // 关闭文件和释放内存
    CloseHandle(hFile);
    VirtualFree(pMemory, 0, MEM_RELEASE);


    //下面是一个写入文件的操作

    return newFileData;
}

int main()
{
    //file_mem
    //file_fix
    WCHAR f1[] = L"/Code/normal/C/VisualStdio/Often/x64/Debug/apt/haha.va";
    WCHAR f2[] = L"/Code/normal/C/VisualStdio/Often/x64/Debug/apt/haha.fa";
    BYTE* data = readFile(f1);
    writeFile(f2, data + 4, *(DWORD*)(data));
    VirtualFree(data, 0, MEM_RELEASE);
}
