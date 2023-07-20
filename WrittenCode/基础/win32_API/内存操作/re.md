





```c
LPVOID VirtualAlloc(
  [in, optional] LPVOID lpAddress,
  [in]           SIZE_T dwSize,
  [in]           DWORD  flAllocationType,
  [in]           DWORD  flProtect
);
```



```
BOOL VirtualProtect(
  [in]  LPVOID lpAddress,
  [in]  SIZE_T dwSize,
  [in]  DWORD  flNewProtect,
  [out] PDWORD lpflOldProtect
);
```







# NtCreateSection

`NtCreateSection` 是 Windows 操作系统的内部函数之一，用于创建一个内存映射文件对象。它是底层的、不公开的函数，用于在用户态和内核态之间进行交互。

通过 `NtCreateSection`，可以创建一个内存映射文件对象，该对象将一个文件映射到进程的地址空间，使得进程可以直接访问文件内容而无需进行显式的文件 I/O 操作。

下面是 `NtCreateSection` 函数的原型：

```c
cppCopy CodeNTSTATUS NtCreateSection(
  PHANDLE           SectionHandle,
  ACCESS_MASK       DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PLARGE_INTEGER    MaximumSize,
  ULONG             SectionPageProtection,
  ULONG             AllocationAttributes,
  HANDLE            FileHandle
);
```

具体各个参数的含义如下：

1.  `SectionHandle`：用于接收创建的内存映射文件对象句柄的指针。
2.  `DesiredAccess`：指定对内存映射文件对象的访问权限和操作类型。
3.  `ObjectAttributes`：指定内存映射文件对象的属性，包括对象名称、访问权限等。
4.  `MaximumSize`：指定映射区域的最大大小。
5.  `SectionPageProtection`：指定内存映射文件对象的保护属性，控制访问权限和内存页属性。
6.  `AllocationAttributes`：指定内存映射文件对象的分配属性，如 SEC_COMMIT、SEC_IMAGE 等。
7.  `FileHandle`：用于创建内存映射文件对象的源文件句柄，可以是打开的文件句柄或者无效句柄。

需要注意的是，`NtCreateSection` 函数属于 Windows 内部函数，平时开发应用程序时不直接使用该函数，而是通过公开的 API 函数如 `CreateFileMapping` 和 `MapViewOfFile` 来进行内存映射文件操作。

希望对你有所帮助，如果还有其他问题，请随时提





# NtMapViewOfSection



`NtMapViewOfSection` 是 Windows 操作系统的内部函数之一，用于将一个内存映射文件对象映射到当前进程的地址空间。它是底层的、不公开的函数，用于在用户态和内核态之间进行交互。

通过 `NtMapViewOfSection`，可以将一个内存映射文件对象映射到当前进程的地址空间，并返回映射后的虚拟内存基地址。

下面是 `NtMapViewOfSection` 函数的原型：

```c
cppCopy CodeNTSTATUS NtMapViewOfSection(
  HANDLE          SectionHandle,
  HANDLE          ProcessHandle,
  PVOID           *BaseAddress,
  ULONG_PTR       ZeroBits,
  SIZE_T          CommitSize,
  PLARGE_INTEGER  SectionOffset,
  PSIZE_T         ViewSize,
  DWORD           InheritDisposition,
  ULONG           AllocationType,
  ULONG           Win32Protect
);
```

具体各个参数的含义如下：

1.  `SectionHandle`：需要映射的内存映射文件对象的句柄。
2.  `ProcessHandle`：当前进程的句柄，用于将内存映射文件对象映射到该进程的地址空间。
3.  `BaseAddress`：用于接收映射后的虚拟内存基地址的指针。
4.  `ZeroBits`：保留字段，必须设置为 0。
5.  `CommitSize`：映射后的虚拟内存区域的初始大小。
6.  `SectionOffset`：指定内存映射文件对象中的偏移量。
7.  `ViewSize`：用于接收映射后的虚拟内存区域的大小的指针。
8.  `InheritDisposition`：指定子进程对该映射的继承方式。
9.  `AllocationType`：指定内存映射文件的分配类型，如 MEM_COMMIT、MEM_RESERVE 等。
10.  `Win32Protect`：指定内存映射文件的保护属性，控制访问权限和内存页属性。

需要注意的是，`NtMapViewOfSection` 函数属于 Windows 内部函数，一般在应用程序开发中并不直接使用它，而是通过公开的 API 函数如 `MapViewOfFile` 来进行内存映射文件操作。

希望对你有所帮助，如果还有其他问题，请随时提问。





`NtUnmapViewOfSection` 是一个 Windows NT 内核函数，用于取消映射已加载到进程地址空间的指定区域。它可以将指定的内存区域从进程的虚拟地址空间中解除映射。

该函数的语法如下所示：

```
CCopy CodeNTSTATUS NtUnmapViewOfSection(
  HANDLE ProcessHandle,
  PVOID  BaseAddress
);
```

其中，`ProcessHandle` 是目标进程的句柄，它表示要解除映射的进程。`BaseAddress` 则是要解除映射的内存区域的基址。

`NtUnmapViewOfSection` 的返回值为 `NTSTATUS` 类型，用于指示函数执行的结果。如果函数成功执行，返回值为 `STATUS_SUCCESS`；否则，返回一个非零的错误代码，代表相应的错误信息。

需要注意的是，`NtUnmapViewOfSection` 是一个内核级函数，通常不建议直接在应用程序中调用。在正常情况下，应使用合适的内存管理函数（例如 `VirtualFreeEx`）来释放进程的内存区域。

请记住，在使用任何内核级函数之前，请确保您具有足够的权限和了解可能的风险。

如果还有其他问题，请随时提问。



# NtProtectVirtualMemory



`NtProtectVirtualMemory` 是 Windows 操作系统的内部函数之一，用于修改指定虚拟内存区域的保护属性。它是底层的、不公开的函数，用于在用户态和内核态之间进行交互。

通过 `NtProtectVirtualMemory`，可以修改指定虚拟内存区域的保护属性，例如修改可读、可写、可执行等权限。

下面是 `NtProtectVirtualMemory` 函数的原型：

```
cppCopy CodeNTSTATUS NtProtectVirtualMemory(
  HANDLE  ProcessHandle,
  PVOID   *BaseAddress,
  PULONG  NumberOfBytesToProtect,
  ULONG   NewAccessProtection,
  PULONG  OldAccessProtection
);
```

具体各个参数的含义如下：

1.  `ProcessHandle`：待操作的进程句柄，表示要修改虚拟内存保护属性的进程。
2.  `BaseAddress`：指向待操作的虚拟内存区域的起始地址的指针。
3.  `NumberOfBytesToProtect`：指定待操作的虚拟内存区域的大小。
4.  `NewAccessProtection`：指定新的保护属性，用于修改虚拟内存区域的访问权限。
5.  `OldAccessProtection`：用于接收修改前的保护属性的指针。

需要注意的是，`NtProtectVirtualMemory` 函数属于 Windows 内部函数，一般在应用程序开发中并不直接使用它，而是通过公开的 API 函数如 `VirtualProtect` 来修改虚拟内存保护属性。

希望对你有所帮助，如果还有其他问题，请随时提问。



`VirtualProtect` 是一个 Windows API 函数，用于更改指定内存区域的访问权限。它可以修改已分配内存页的保护属性，如可执行、可读、可写和可执行等。

函数原型为：

```
cCopy CodeBOOL VirtualProtect(
  LPVOID lpAddress,
  SIZE_T dwSize,
  DWORD  flNewProtect,
  PDWORD lpflOldProtect
);
```

参数说明：

-   `lpAddress`：要修改访问权限的内存区域的起始地址。

-   `dwSize`：要修改访问权限的内存区域的大小。

-   ```
    flNewProtect
    ```

    ：新的访问权限的属性，可以是以下常量之一：

    -   PAGE_EXECUTE：可执行权限。
    -   PAGE_EXECUTE_READ：可执行和可读权限。
    -   PAGE_EXECUTE_READWRITE：可执行、可读和可写权限。
    -   PAGE_EXECUTE_WRITECOPY：可执行、可写和写时复制权限。
    -   PAGE_NOACCESS：无访问权限。
    -   PAGE_READONLY：只读权限。
    -   PAGE_READWRITE：可读和可写权限。
    -   PAGE_WRITECOPY：可写和写时复制权限。

-   `lpflOldProtect`：用于保存旧的访问权限的变量的指针。

`VirtualProtect` 函数可以用于保护内存区域的安全性或灵活地更改内存区域的权限，但使用该函数需要谨慎，并且必须具有适当的权限。在实际使用中，请确保了解并遵循操作系统和应用程序开发的最佳实践。

# GetModuleInformation



`GetModuleInformation` 是一个 Windows API 函数，用于检索指定模块的详细信息。它可以获取某个进程加载的模块的基址、大小和路径等信息。

下面是 `GetModuleInformation` 函数的原型：

```
cppCopy CodeBOOL GetModuleInformation(
  HANDLE               hProcess,
  HMODULE              hModule,
  LPMODULEINFO         lpmodinfo,
  DWORD                cb
);
```

具体各个参数的含义如下：

1.  `hProcess`：指定要获取模块信息的进程的句柄。
2.  `hModule`：指定要获取信息的模块的句柄。如果传入 NULL，则表示获取当前进程的信息。
3.  `lpmodinfo`：指向 `MODULEINFO` 结构体的指针，用于接收模块的详细信息。
4.  `cb`：`lpmodinfo` 缓冲区的大小，即 `MODULEINFO` 结构体的字节数。

`MODULEINFO` 结构体包含以下成员：

```
cppCopy Codetypedef struct _MODULEINFO {
  LPVOID   lpBaseOfDll;
  DWORD    SizeOfImage;
  LPVOID   EntryPoint;
} MODULEINFO, *LPMODULEINFO;
```

其中，`lpBaseOfDll` 是模块的基址（起始地址），`SizeOfImage` 是模块的大小，`EntryPoint` 是模块的入口点地址。

通过调用 `GetModuleInformation` 函数，可以检索指定模块的详细信息，并将结果保存在 `MODULEINFO` 结构体中。

希望对你有所帮助，如果还有其他问题，请随时提问。







# eg







```c
#include <windows.h>
#include <stdio.h>

int main() {
    // 分配内存
    LPVOID pMemory = VirtualAlloc(
        NULL,               // 指定分配的地址，在此使用 NULL 表示由系统选择
        4096,               // 分配的内存大小（单位为字节），这里分配 4KB
        MEM_COMMIT,         // 内存分配类型，表示提交内存
        PAGE_READWRITE      // 内存页面属性，读写权限
    );
    if (pMemory == NULL) {
        printf("无法分配内存\n");
        return 1;
    }

    // 修改内存的保护属性
    DWORD oldProtect;
    if (!VirtualProtect(
        pMemory,            // 指向要修改保护属性的内存地址
        4096,               // 内存大小（单位为字节），与分配时一致
        PAGE_EXECUTE_READ,  // 新的内存页面属性，可读可执行权限
        &oldProtect         // 用于保存原来的保护属性
    )) {
        printf("修改内存保护属性失败\n");
        VirtualFree(pMemory, 0, MEM_RELEASE);
        return 1;
    }

    // 在这里可以对内存进行读写和执行操作
    // ...

    // 释放内存
    if (!VirtualFree(pMemory, 0, MEM_RELEASE)) {
        printf("释放内存失败\n");
        return 1;
    }

    return 0;
}

```





