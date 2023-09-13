# [10] 加载配置表 | 异常处理表



根据自己写的exe,发现好像那个表没什么明显的用处

书上讲了很多和内核相关的东西,暂时看不懂(也就是和内核的一些关键函数和流程的分析)

和SEH有关:

![Untitled](img/7dd3496d4e184434a98edc11de043e96Untitled16.png)

ExceptionList:

![Untitled](img/7dd3496d4e184434a98edc11de043e96Untitled17.png)



下面这个表和书上的表有点不同哟

大体相同

```c
typedef struct _IMAGE_LOAD_CONFIG_DIRECTORY32 {
    DWORD   Size;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;//主版本号
    WORD    MinorVersion;//次版本号
    DWORD   GlobalFlagsClear;//标志1 需要被清除的
    DWORD   GlobalFlagsSet;//标志2 需要被初始化的
    DWORD   CriticalSectionDefaultTimeout;//最大超时限制
    DWORD   DeCommitFreeBlockThreshold;//释放内存数量
    DWORD   DeCommitTotalFreeThreshold;//空闲内存数量
    DWORD   LockPrefixTable;                // VA 使用locl前缀的指令地址
    DWORD   MaximumAllocationSize;//最大分配粒度
    DWORD   VirtualMemoryThreshold;//最大虚拟内存大小
    DWORD   ProcessHeapFlags;//进程堆标志
    DWORD   ProcessAffinityMask;//函数参数
    WORD    CSDVersion;//版本标识
    WORD    DependentLoadFlags;
    DWORD   EditList;                       // VA cookies指针
    DWORD   SecurityCookie;                 // VA 指向safe handler 处理程序列表
    DWORD   SEHandlerTable;                 // VA
    DWORD   SEHandlerCount;//safe handler
    DWORD   GuardCFCheckFunctionPointer;    // VA
    DWORD   GuardCFDispatchFunctionPointer; // VA
    DWORD   GuardCFFunctionTable;           // VA
    DWORD   GuardCFFunctionCount;
    DWORD   GuardFlags;
    IMAGE_LOAD_CONFIG_CODE_INTEGRITY CodeIntegrity;
    DWORD   GuardAddressTakenIatEntryTable; // VA
    DWORD   GuardAddressTakenIatEntryCount;
    DWORD   GuardLongJumpTargetTable;       // VA
    DWORD   GuardLongJumpTargetCount;
    DWORD   DynamicValueRelocTable;         // VA
    DWORD   CHPEMetadataPointer;
    DWORD   GuardRFFailureRoutine;          // VA
    DWORD   GuardRFFailureRoutineFunctionPointer; // VA
    DWORD   DynamicValueRelocTableOffset;
    WORD    DynamicValueRelocTableSection;
    WORD    Reserved2;
    DWORD   GuardRFVerifyStackPointerFunctionPointer; // VA
    DWORD   HotPatchTableOffset;
    DWORD   Reserved3;
    DWORD   EnclaveConfigurationPointer;    // VA
    DWORD   VolatileMetadataPointer;        // VA
    DWORD   GuardEHContinuationTable;       // VA
    DWORD   GuardEHContinuationCount;
    DWORD   GuardXFGCheckFunctionPointer;    // VA
    DWORD   GuardXFGDispatchFunctionPointer; // VA
    DWORD   GuardXFGTableDispatchFunctionPointer; // VA
    DWORD   CastGuardOsDeterminedFailureMode; // VA
} IMAGE_LOAD_CONFIG_DIRECTORY32, *PIMAGE_LOAD_CONFIG_DIRECTORY32;
```



