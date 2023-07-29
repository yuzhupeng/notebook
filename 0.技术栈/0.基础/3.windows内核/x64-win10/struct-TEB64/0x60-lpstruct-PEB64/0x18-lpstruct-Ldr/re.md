# 介绍

这个成员本身是一个指针,指向了一个结构体**_PEB_LDR_DATA**

```c
//0x58 bytes (sizeof)
struct _PEB_LDR_DATA
{
    ULONG Length;                                                           //0x0 结构体大小
    UCHAR Initialized;                                                      //0x4 进程是否初始化完成
    VOID* SsHandle;                                                         //0x8 具体用途可能与子系统相关
    struct _LIST_ENTRY InLoadOrderModuleList;           //0x10 表示按照加载顺序排列的模块列表
    struct _LIST_ENTRY InMemoryOrderModuleList;     //0x20 表示按照内存顺序排列的模块列表
    struct _LIST_ENTRY InInitializationOrderModuleList;   //0x30 表示按照初始化顺序排列的模块列表
    VOID* EntryInProgress;                                                  //0x40 可能与正在进行的进程入口点相关
    UCHAR ShutdownInProgress;                                        //0x48 用于指示是否正在进行关闭操
    VOID* ShutdownThreadId;                                              //0x50 可能与关闭操作的线程ID相关
};
```

# 0x10;0x20;0x30

struct _LIST_ENTRY InLoadOrderModuleList;           //0x10 表示按照加载顺序排列的模块列表

struct _LIST_ENTRY InMemoryOrderModuleList;     //0x20 表示按照内存顺序排列的模块列表

struct _LIST_ENTRY InInitializationOrderModuleList;   //0x30 表示按照初始化顺序排列的模块列表

首先他是一个结构体

```c
//0x10 bytes (sizeof)
struct _LIST_ENTRY
{
    struct _LIST_ENTRY* Flink;                                              //0x0
    struct _LIST_ENTRY* Blink;                                              //0x8
};
```

我很奇怪的是,这个链表没有值域,只有指针域

或者说,有值域,我不会看

```c
//0x10 bytes (sizeof)
struct _LIST_ENTRY
{
    struct _LIST_ENTRY* Flink;                                              //0x0
    struct _LIST_ENTRY* Blink;                                              //0x8
		VOID* lp_DAta; //值域
};
```

查官网说,链表的指向的结构体如图所示 LDR_DATA_TABLE_ENTRY

 