typedef LONG KPRIORITY;
typedef enum _SYSTEM_INFORMATION_CLASS {

	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemNextEventIdInformation,
	SystemEventIdsInformation,
	SystemCrashDumpInformation,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemPlugPlayBusInformation,
	SystemDockInformation,
	SystemPowerInformation2,
	SystemProcessorSpeedInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation

} SYSTEM_INFORMATION_CLASS;
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;
typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, * PCLIENT_ID;
typedef struct _SYSTEM_THREAD_INFORMATION
{
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	LONG BasePriority;
	ULONG ContextSwitches;
	ULONG ThreadState;
	ULONG WaitReason;
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;
typedef struct _SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;//下一个进程信息的偏移量，允许遍历进程信息链表。
	ULONG NumberOfThreads;//进程中的线程数。
	LARGE_INTEGER SpareLi1;//8
	LARGE_INTEGER SpareLi2;//8
	LARGE_INTEGER SpareLi3;//8
	LARGE_INTEGER CreateTime;//进程创建时间
	LARGE_INTEGER UserTime;//进程执行在用户模式的时间
	LARGE_INTEGER KernelTime;//进程执行在内核模式的时间
	UNICODE_STRING ImageName;//进程的可执行文件路径
	KPRIORITY BasePriority;//进程的基本优先级
	HANDLE UniqueProcessId;//进程的唯一ID。
	HANDLE InheritedFromUniqueProcessId;//继承进程的唯一ID
	ULONG HandleCount;//进程当前打开的句柄数量。
	ULONG SessionId;//进程所属的会话ID
	ULONG_PTR PageDirectoryBase;//进程的唯一键
	SIZE_T PeakVirtualSize;//进程的峰值虚拟内存大小
	SIZE_T VirtualSize;//进程的虚拟内存大小
	ULONG PageFaultCount;//进程的页面错误次数
	SIZE_T PeakWorkingSetSize;//进程的峰值工作集大小
	SIZE_T WorkingSetSize;//进程的工作集大小
	SIZE_T QuotaPeakPagedPoolUsage;//进程的峰值分页池使用量
	SIZE_T QuotaPagedPoolUsage;//进程的分页池使用量
	SIZE_T QuotaPeakNonPagedPoolUsage;//进程的峰值非分页池使用量
	SIZE_T QuotaNonPagedPoolUsage;//进程的非分页池使用量
	SIZE_T PagefileUsage;//进程的页面文件使用量
	SIZE_T PeakPagefileUsage;//进程的峰值页面文件使用量
	SIZE_T PrivatePageCount;//进程的私有页面数量
	LARGE_INTEGER ReadOperationCount;//进程的读取操作计数
	LARGE_INTEGER WriteOperationCount;//进程的写入操作计数
	LARGE_INTEGER OtherOperationCount;//进程的其他操作计数
	LARGE_INTEGER ReadTransferCount;//进程的读取传输计数。
	LARGE_INTEGER WriteTransferCount;//进程的写入传输计数
	LARGE_INTEGER OtherTransferCount;//进程的其他传输计数
	SYSTEM_THREAD_INFORMATION Threads[1];//柔性数组
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef long (WINAPI* type_NtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );
