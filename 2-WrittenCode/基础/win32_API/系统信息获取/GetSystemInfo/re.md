

# eg1



```c
#include <stdio.h>
#include <Windows.h>

int main() {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);

    printf("操作系统制造商标识符: %u\n", systemInfo.dwOemId);
    printf("页面大小（字节）: %u\n", systemInfo.dwPageSize);
    printf("最小可用内存地址: 0x%p\n", systemInfo.lpMinimumApplicationAddress);
    printf("最大可用内存地址: 0x%p\n", systemInfo.lpMaximumApplicationAddress);
    printf("可用处理器掩码: 0x%Ix\n", systemInfo.dwActiveProcessorMask);
    printf("逻辑处理器数量: %u\n", systemInfo.dwNumberOfProcessors);
    printf("处理器类型: %x\n", systemInfo.dwProcessorType);
    printf("内存分配的最小单位: %u\n", systemInfo.dwAllocationGranularity);
    printf("处理器级别: %x\n", systemInfo.wProcessorLevel);
    printf("处理器修订级别: %x\n", systemInfo.wProcessorRevision);

    return 0;
}
  
```

