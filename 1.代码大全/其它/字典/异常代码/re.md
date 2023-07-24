

# 异常表总览

```c
/*lint -save -e767 */  
#define STATUS_WAIT_0                           ((DWORD   )0x00000000L) 
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)    
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define DBG_EXCEPTION_HANDLED            ((DWORD   )0x00010001L)    
#define DBG_CONTINUE                     ((DWORD   )0x00010002L)    
#define STATUS_SEGMENT_NOTIFICATION      ((DWORD   )0x40000005L)    
#define STATUS_FATAL_APP_EXIT            ((DWORD   )0x40000015L)    
#define DBG_TERMINATE_THREAD             ((DWORD   )0x40010003L)    
#define DBG_TERMINATE_PROCESS            ((DWORD   )0x40010004L)    
#define DBG_CONTROL_C                    ((DWORD   )0x40010005L)    
#define DBG_PRINTEXCEPTION_C             ((DWORD   )0x40010006L)    
#define DBG_RIPEXCEPTION                 ((DWORD   )0x40010007L)    
#define DBG_CONTROL_BREAK                ((DWORD   )0x40010008L)    
#define DBG_COMMAND_EXCEPTION            ((DWORD   )0x40010009L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)    
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)    
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)    
#define STATUS_LONGJUMP                  ((DWORD   )0x80000026L)    
#define STATUS_UNWIND_CONSOLIDATE        ((DWORD   )0x80000029L)    
#define DBG_EXCEPTION_NOT_HANDLED        ((DWORD   )0x80010001L)    
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
#define STATUS_INVALID_PARAMETER         ((DWORD   )0xC000000DL)    
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
#define STATUS_DLL_NOT_FOUND             ((DWORD   )0xC0000135L)    
#define STATUS_ORDINAL_NOT_FOUND         ((DWORD   )0xC0000138L)    
#define STATUS_ENTRYPOINT_NOT_FOUND      ((DWORD   )0xC0000139L)    
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
#define STATUS_DLL_INIT_FAILED           ((DWORD   )0xC0000142L)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
#define STATUS_REG_NAT_CONSUMPTION       ((DWORD   )0xC00002C9L)    
#define STATUS_HEAP_CORRUPTION           ((DWORD   )0xC0000374L)    
#define STATUS_STACK_BUFFER_OVERRUN      ((DWORD   )0xC0000409L)    
#define STATUS_INVALID_CRUNTIME_PARAMETER ((DWORD   )0xC0000417L)    
#define STATUS_ASSERTION_FAILURE         ((DWORD   )0xC0000420L)    
#if defined(STATUS_SUCCESS) || (_WIN32_WINNT > 0x0500) || (_WIN32_FUSION >= 0x0100) 
#define STATUS_SXS_EARLY_DEACTIVATION    ((DWORD   )0xC015000FL)    
#define STATUS_SXS_INVALID_DEACTIVATION  ((DWORD   )0xC0150010L)    
#endif 
/*lint -restore */
```



常见的异常

```c
EXCEPTION_DATATYPE_MISALIGNMENT        (0x80000002)   程序读取一个未经对齐的数据时引发的异常。
EXCEPTION_BREAKPOINT                   (0x80000003)   断点异常
EXCEPTION_SINGLE_STEP                  (0x80000004)   标志寄存器的TF位为1时，每执行一条指令就会引发该异常
EXCEPTION_IN_PAGE_ERROR                (0x80000006)
EXCEPTION_ILLEGAL_INSTRUCTION          (0x8000001D)   无法识别的指令
EXCEPTION_NONCONTINUABLE_EXCEPTION     (0x80000025)
EXCEPTION_INVALID_DISPOSITION          (0x80000026)
EXCEPTION_ARRAY_BOUNDS_EXCEEDED        (0x8000008C)
EXCEPTION_FLT_DENORMAL_OPERAND         (0x8000008D)
EXCEPTION_INT_DIVIDE_BY_ZERO           (0x80000094)   除法运算分母为0

EXCEPTION_ACCESS_VIOLATION     			0xC0000005     
//程序企图读写一个不可访问的地址时引发的异常。例如企图读取0地址处的内存。

EXCEPTION_ARRAY_BOUNDS_EXCEEDED    		0xC000008C     
//数组访问越界时引发的异常

EXCEPTION_FLT_DENORMAL_OPERAND     		0xC000008D     
//如果浮点数操作的操作数是非正常的，则引发该异常。所谓非正常，即它的值太小以至于不能用标准格式表示出来。

EXCEPTION_FLT_DIVIDE_BY_ZERO            0xC000008E     浮点数除法的除数是0时引发该异常。  
EXCEPTION_FLT_INEXACT_RESULT            0xC000008F     浮点数操作的结果不能精确表示成小数时引发该异常。  
EXCEPTION_FLT_INVALID_OPERATION         0xC0000090     该异常表示不包括在这个表内的其它浮点数异常。  
EXCEPTION_FLT_OVERFLOW                  0xC0000091     浮点数的指数超过所能表示的最大值时引发该异常。  
EXCEPTION_FLT_STACK_CHECK               0xC0000092     进行浮点数运算时栈发生溢出或下溢时引发该异常。   
EXCEPTION_FLT_UNDERFLOW                 0xC0000093     浮点数的指数小于所能表示的最小值时引发该异常。   
EXCEPTION_ILLEGAL_INSTRUCTION           0xC000001D     程序企图执行一个无效的指令时引发该异常。   
EXCEPTION_IN_PAGE_ERROR                 0xC0000006     程序要访问的内存页不在物理内存中时引发的异常。
EXCEPTION_INT_DIVIDE_BY_ZERO            0xC0000094     整数除法的除数是0时引发该异常。
EXCEPTION_INT_OVERFLOW                  0xC0000095     整数操作的结果溢出时引发该异常。
EXCEPTION_INVALID_DISPOSITION           0xC0000026     异常处理器返回一个无效的处理的时引发该异常。
EXCEPTION_NONCONTINUABLE_EXCEPTION      0xC0000025     发生一个不可继续执行的异常时，如果程序继续执行，则会引发该异常。
EXCEPTION_PRIV_INSTRUCTION              0xC0000096     程序企图执行一条当前CPU模式不允许的指令时引发该异常。
EXCEPTION_STACK_OVERFLOW                0xC00000FD     栈溢出时引发该异常。
```

