其实就是每个进程都会有一个这样的结构体去记录自身加载了哪些模块

通过加载的模块,就可以获取他们的基址



```c
//0x120 bytes (sizeof)
struct _LDR_DATA_TABLE_ENTRY
{
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0 ，表示根据加载顺序链接的模块列表中的链表节点
    struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x10 表示根据内存顺序链接的模块列表中的链表节点
    struct _LIST_ENTRY InInitializationOrderLinks;                          //0x20 表示根据初始化顺序链接的模块列表中的链表节点
    VOID* DllBase;                                                          //0x30 指向模块的基址
    VOID* EntryPoint;                                                       //0x38 指向模块的入口点
    ULONG SizeOfImage;                                                      //0x40 表示模块的映像大小
    struct _UNICODE_STRING FullDllName;                                     //0x48 表示模块的完整路径名
    struct _UNICODE_STRING BaseDllName;                                     //0x58 表示模块的基本文件名。
    union //4字节
    {
        UCHAR FlagGroup[4];                                                 //0x68  //用于表示一组标志
        ULONG Flags;                                                        //0x68 表示模块的多个标志。
        struct
        {
            ULONG PackagedBinary:1;                                         //0x68
            ULONG MarkedForRemoval:1;                                       //0x68
            ULONG ImageDll:1;                                               //0x68
            ULONG LoadNotificationsSent:1;                                  //0x68
            ULONG TelemetryEntryProcessed:1;                                //0x68
            ULONG ProcessStaticImport:1;                                    //0x68
            ULONG InLegacyLists:1;                                          //0x68
            ULONG InIndexes:1;                                              //0x68
            ULONG ShimDll:1;                                                //0x68
            ULONG InExceptionTable:1;                                       //0x68
            ULONG ReservedFlags1:2;                                         //0x68
            ULONG LoadInProgress:1;                                         //0x68
            ULONG LoadConfigProcessed:1;                                    //0x68
            ULONG EntryProcessed:1;                                         //0x68
            ULONG ProtectDelayLoad:1;                                       //0x68
            ULONG ReservedFlags3:2;                                         //0x68
            ULONG DontCallForThreads:1;                                     //0x68
            ULONG ProcessAttachCalled:1;                                    //0x68
            ULONG ProcessAttachFailed:1;                                    //0x68
            ULONG CorDeferredValidate:1;                                    //0x68
            ULONG CorImage:1;                                               //0x68
            ULONG DontRelocate:1;                                           //0x68
            ULONG CorILOnly:1;                                              //0x68
            ULONG ChpeImage:1;                                              //0x68
            ULONG ReservedFlags5:2;                                         //0x68
            ULONG Redirected:1;                                             //0x68
            ULONG ReservedFlags6:2;                                         //0x68
            ULONG CompatDatabaseProcessed:1;                                //0x68
        };
    };
    USHORT ObsoleteLoadCount;                                               //0x6c 表示模块的过期加载计数
    USHORT TlsIndex;                                                        //0x6e 表示模块的线程局部存储（TLS）索引
    struct _LIST_ENTRY HashLinks;                                           //0x70 用于哈希链接到模块的链表节点
    ULONG TimeDateStamp;                                                    //0x80 表示模块的时间戳
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x88 表示模块入口点的激活上下文
    VOID* Lock;                                                             //0x90 可能与模块加载相关的锁定机制有关
    struct _LDR_DDAG_NODE* DdagNode;                                        //0x98 可能与模块之间的依赖关系有关
    struct _LIST_ENTRY NodeModuleLink;                                      //0xa0 用于链接到模块节点的链表
    struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0xb0 可能与模块加载上下文有关
    VOID* ParentDllBase;                                                    //0xb8 指向父模块的基址
    VOID* SwitchBackContext;                                                //0xc0 ，可能与切换上下文相关
    struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0xc8 可能与基地址索引节点相关
    struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0xe0 可能与映射信息索引节点相关
    ULONGLONG OriginalBase;                                                 //0xf8 表示原始基址
    union _LARGE_INTEGER LoadTime;                                          //0x100 表示模块加载时间
    ULONG BaseNameHashValue;                                                //0x108  表示模块基本文件名的哈希值
    enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x10c 表示模块的加载原因。
    ULONG ImplicitPathOptions;                                              //0x110 表示隐式路径选项。
    ULONG ReferenceCount;                                                   //0x114 表示模块的引用计数
    ULONG DependentLoadFlags;                                               //0x118 表示依赖加载的标志
    UCHAR SigningLevel;                                                     //0x11c 表示模块的签名级别
};
```