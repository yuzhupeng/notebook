

# CreateFileW



`CreateFileW` 是一个 Windows API 函数，用于创建或打开一个文件，并返回一个与该文件关联的句柄（`HANDLE`）。下面是 `CreateFileW` 函数的参数及其解释：

```
cppCopy CodeHANDLE CreateFileW(
  LPCWSTR               lpFileName,
  DWORD                 dwDesiredAccess,
  DWORD                 dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD                 dwCreationDisposition,
  DWORD                 dwFlagsAndAttributes,
  HANDLE                hTemplateFile
);
```

1.  `lpFileName`：指定要创建或打开的文件的路径。这是一个以 null 结尾的 Unicode 字符串，使用 `LPCWSTR` 类型传递。

2.  ```
    dwDesiredAccess
    ```

    ：指定对文件的访问权限和操作类型。可以通过对不同的常量值进行位运算来指定所需的访问权限。常见的访问权限常量包括：

    -   `GENERIC_READ`：可读取文件。
    -   `GENERIC_WRITE`：可写入文件。

3.  ```
    dwShareMode
    ```

    ：指定其他进程对文件的共享方式。常见的共享模式常量包括：

    -   `FILE_SHARE_READ`：允许其他进程读取文件。
    -   `FILE_SHARE_WRITE`：允许其他进程写入文件。

4.  `lpSecurityAttributes`：指定文件的安全属性。通常情况下，传入 `NULL` 表示使用默认安全属性。

5.  ```
    dwCreationDisposition
    ```

    ：指定打开或创建文件的行为和操作类型。常见的取值包括：

    -   `CREATE_NEW`：创建新文件，如果文件已存在则创建失败。
    -   `CREATE_ALWAYS`：创建新文件，如果文件已存在则覆盖。
    -   `OPEN_EXISTING`：打开已存在的文件，如果文件不存在则失败。
    -   `OPEN_ALWAYS`：打开已存在的文件，如果文件不存在则创建新文件。
    -   `TRUNCATE_EXISTING`：截断已存在的文件，将其大小设置为 0 字节。

6.  ```
    dwFlagsAndAttributes
    ```

    ：指定打开或创建文件的特殊标志和属性。常见的标志和属性常量包括：

    -   `FILE_ATTRIBUTE_NORMAL`：普通文件属性，没有特殊标志。
    -   `FILE_FLAG_WRITE_THROUGH`：直接将数据写入物理媒体，而不是写入缓存。
    -   `FILE_FLAG_DELETE_ON_CLOSE`：在关闭文件句柄时删除该文件。

7.  `hTemplateFile`：指定一个文件句柄的模板。通常传入 `NULL`。

这些参数共同决定了 `CreateFileW` 函数的行为，用于创建或打开文件并获取与文件相关联的句柄。

请注意，使用 `CreateFileW` 函数时，需要在代码中包含 `Windows.h` 头文件，并链接相应的库文件。

如果还有其他问题，请随时提问。



# NTCreateFile

`NTCreateFile` 是 Windows 操作系统的内部函数，用于创建或打开一个文件，并返回一个与该文件关联的句柄。它是底层的、不公开的函数，用于操作文件系统。

`NTCreateFile` 函数与用户态的 `CreateFileW` 函数相似，但更为底层和复杂。它直接与 Windows 内核进行交互，提供了更多的灵活性和功能，但使用起来也更加复杂。

下面是 `NTCreateFile` 函数的原型：

```
cppCopy CodeNTSTATUS NTCreateFile(
  PHANDLE            FileHandle,
  ACCESS_MASK        DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PIO_STATUS_BLOCK   IoStatusBlock,
  PLARGE_INTEGER     AllocationSize,
  ULONG              FileAttributes,
  ULONG              ShareAccess,
  ULONG              CreateDisposition,
  ULONG              CreateOptions,
  PVOID              EaBuffer,
  ULONG              EaLength
);
```

具体各个参数的含义如下：

1.  `FileHandle`：用于接收创建或打开的文件句柄的指针。
2.  `DesiredAccess`：指定对文件的访问权限和操作类型，使用访问掩码（ACCESS_MASK）来表示。
3.  `ObjectAttributes`：指定文件对象的属性，包括文件名、访问权限等。它是一个 `OBJECT_ATTRIBUTES` 结构体指针。
4.  `IoStatusBlock`：用于接收操作状态的信息的结构体指针。
5.  `AllocationSize`：指定新文件的初始大小或重定位已存在文件的大小。
6.  `FileAttributes`：指定新文件的文件属性，如只读、隐藏、系统等。
7.  `ShareAccess`：指定其他进程对文件的共享方式。
8.  `CreateDisposition`：指定打开或创建文件的行为和操作类型。
9.  `CreateOptions`：指定文件的创建选项和行为，如同步异步 I/O、缓存方式等。
10.  `EaBuffer`：指向扩展属性列表的缓冲区的指针。
11.  `EaLength`：扩展属性列表的长度。

需要注意的是，`NTCreateFile` 函数的参数和行为与具体的 Windows 版本和内核实现相关，因此在不同的环境下可能会有所变化。

由于 `NTCreateFile` 是操作系统内部函数，一般情况下在应用程序开发中并不直接使用它，而是通过公开的 API 函数如 `CreateFileW` 来进行文件操作。

希望能对你有所帮助，如果还有其他问题，请随时提问。





# Eg



## 读取到内存



```c
#include <windows.h>
#include <stdio.h>
BYTE* read2Mem(char* fpath)
{
    // 打开文件
    DWORD fileSize;
    LPVOID pMemory;
    DWORD bytesRead;

    HANDLE hFile = CreateFileA(fpath,  // 文件路径，根据实际情况进行修改
        GENERIC_READ,               // 访问模式，只读
        0,                          // 共享模式，不共享
        NULL,                       // 安全特性，使用默认值
        OPEN_EXISTING,              // 打开已存在的文件
        FILE_ATTRIBUTE_NORMAL,      // 文件属性，正常
        NULL                        // 模板文件句柄，不使用
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("无法打开文件\n");
        return NULL;
    }

    // 获取文件大小
    fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) 
    {
        printf("无法获取文件大小\n");
        CloseHandle(hFile);
        return NULL;
    }

    // 分配内存
    pMemory = VirtualAlloc(
        NULL,                   // 指定分配的地址，在此使用 NULL 表示由系统选择
        fileSize,               // 分配的内存大小，与文件大小一致
        MEM_COMMIT,             // 内存分配类型，表示提交内存
        PAGE_READWRITE          // 内存页面属性，读写权限
    );
    if (pMemory == NULL) 
    {
        printf("无法分配内存\n");
        CloseHandle(hFile);
        return NULL;
    }

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
        return NULL;
    }
    CloseHandle(hFile);
    //VirtualFree(pMemory, 0, MEM_RELEASE);
    return pMemory;
}
int main() {

    char fpath[] = "D:/misc/level1.exe";
    char* lpTmp;
    lpTmp = read2Mem(fpath);
    return 0;
}


```



## 写入





```c
#include <stdio.h>
#include <windows.h>

int main() {
    HANDLE hFile;
    DWORD dwBytesWritten;
    const char* dataToWrite = "Hello, this is a test file.\r\n";

    // 创建文件，如果文件存在则打开，如果文件不存在则创建
    hFile = CreateFileA(
        "test.txt",             // 文件名
        GENERIC_WRITE,          // 写入访问权限
        0,                      // 共享模式（0表示不共享）
        NULL,                   // 安全属性（使用默认值）
        CREATE_ALWAYS,          // 文件不存在时创建，如果存在则覆盖
        FILE_ATTRIBUTE_NORMAL,  // 文件属性（正常文件）
        NULL                    // 模板文件句柄（不使用模板）
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("文件创建或打开失败\n");
        return 1;
    }

    // 写入数据到文件
    if (!WriteFile(hFile, dataToWrite, strlen(dataToWrite), &dwBytesWritten, NULL)) {
        printf("写入文件失败\n");
        CloseHandle(hFile);
        return 1;
    }

    printf("成功写入 %d 字节到文件\n", dwBytesWritten);

    // 关闭文件句柄
    CloseHandle(hFile);

    return 0;
}

```

