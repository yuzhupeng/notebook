



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

