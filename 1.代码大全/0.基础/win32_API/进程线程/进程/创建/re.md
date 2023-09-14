

# API

## CreateProcess 



```c
BOOL WINAPI CreateProcess(
  __in_opt     LPCTSTR lpApplicationName, //应用程序的字符串名字
  __inout_opt  LPTSTR lpCommandLine,	//对应的命令行参数 
  __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,// 与CreateProcess创建的进程的进程句柄是否可以继承有关
  __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes, // 与CreateProcess创建的进程的线程句柄是否可以继承有关
  __in         BOOL bInheritHandles, //创建子进程的时候,是否[需要]继承父进程的句柄表中能够继承的部分,
    								//只有那些父进程句柄表中安全属性结构体SECURITY_ATTRIBUTES::bInheritHandle = TRUE;
    								//子进程创建的时候,参数5 InheritHandles=true,才代表一定传递下去
  __in         DWORD dwCreationFlags, //出现的形式,如果为null,子进程将会使用父进程的控制台
  __in_opt     LPVOID lpEnvironment,
  __in_opt     LPCTSTR lpCurrentDirectory,//如果为NULL,那么子进程的argv[0]将会是父进程的地址,你可以为子进程指定一个字符串地址
  __in         LPSTARTUPINFO lpStartupInfo, //exe以怎样的方式出现
  __out        LPPROCESS_INFORMATION lpProcessInformation//会返回 进程句柄,主线程句柄,进程ID,线程ID....
);
```

**返回值**

如果函数执行成功，返回非零值。

如果函数执行失败，返回零，可以使用GetLastError函数获得错误的附加信息。



example 下面这个程序就是用于创建一个进程,并等待该进程结束的

```c
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

void _tmain( int argc, TCHAR *argv[] )
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if( argc != 2 )
    {
        printf("Usage: %s [cmdline]\\n", argv[0]);
        return;
    }

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        argv[1],        // Command line 
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
```



### d**wCreationFlags常见属性**



指定附加的、用来控制优先类和进程的创建的标志。以下的创建标志可以以除下面列出的方式外的任何方式组合后指定。

⑴值：**CREATE_DEFAULT_ERROR_MODE (看不懂)**

含义：新的进程不继承调用进程的错误模式。**CreateProcess**函数赋予新进程当前的默认错误模式作为替代。

应用程序可以调用SetErrorMode函数设置当前的默认错误模式。

这个标志对于那些运行在没有硬件错误环境下的多线程[外壳程序](https://baike.baidu.com/item/外壳程序)是十分有用的。

对于**CreateProcess**函数，默认的行为是为新进程继承调用者的错误模式。设置这个标志以改变默认的处理方式。

⑵值：**CREATE_NEW_CONSOLE**

含义：新的进程将使用一个新的控制台，而不是继承父进程的控制台。这个标志不能与DETACHED_PROCESS标志一起使用。

⑶值：**CREATE_NEW_PROCESS_GROUP**

含义：新进程将是一个进程树的根进程。进程树中的全部进程都是根进程的子进程。新进程树的用户标识符与这个进程的标识符是相同的，

由lpProcessInformation参数返回。进程树经常使用GenerateConsoleCtrlEvent函数允许发送CTRL+C或CTRL+BREAK信号到一组控制台进程。

⑷值：**CREATE_SEPARATE_WOW_VDM (看不懂)**

如果被设置，新进程将会在一个私有的虚拟DOS机（VDM）中运行。另外，默认情况下所有的16位Windows应用程序都会在

同一个共享的VDM中以线程的方式运行。单独运行一个16位程序的优点是一个应用程序的崩溃只会结束这一个VDM的运行；

其他那些在不同VDM中运行的程序会继续正常的运行。

同样的，在不同VDM中运行的16位Windows应用程序拥有不同的输入队列，这意味着如果一个程序暂时失去响应，在独立的VDM中的应用程序能够继续获得输入。

⑸值：**CREATE_SHARED_WOW_VDM (看不懂)**

如果WIN.INI中的Windows段的DefaultSeparateVDM选项被设置为真，这个标识使得CreateProcess函数越过这个选项并在共享的虚拟DOS机中运行新进程。

⑹值：**CREATE_SUSPENDED**

含义：新进程的主线程会以暂停的状态被创建，直到调用ResumeThread函数被调用时才运行。

⑺值：**CREATE_UNICODE_ENVIRONMENT**

含义：如果被设置，由lpEnvironment参数指定的环境块使用Unicode字符，如果为空，环境块使用ANSI字符。

⑻值：**DEBUG_PROCESS**

含义：如果这个标志被设置，调用进程将被当做一个调试程序，并且新进程会被当做被调试的进程。系统把被调试程序发生的所有调试事件通知给调试器。

如果你使用这个标志创建进程，只有调用进程（调用CreateProcess函数的进程）可以调用WaitForDebugEvent函数。

⑼值：**DEBUG_ONLY_THIS_PROCESS (看不懂)**

含义：如果此标志没有被设置且调用进程正在被调试，新进程将成为调试调用进程的调试器的另一个调试对象。

如果调用进程没有被调试，有关调试的行为就不会产生。

⑽值：**DETACHED_PROCESS**

含义：对于控制台进程，新进程没有访问父进程控制台的权限。新进程可以通过AllocConsole函数自己创建一个新的控制台。

个标志不可以与CREATE_NEW_CONSOLE标志一起使用。

〔11〕值：**CREATE_NO_WINDOW**

含义：系统不为新进程创建CUI窗口，使用该标志可以创建不含窗口的CUI程序。

### **dwCreationFlags**参数设置优先级

还用来控制新进程的优先类，优先类用来决定此进程的线程调度的优先级。

如果下面的优先级类标志都没有被指定，那么默认的优先类是**NORMAL_PRIORITY_CLASS**，

除非被创建的进程是**IDLE_PRIORITY_CLASS**。在这种情况下子进程的默认优先类是**IDLE_PRIORITY_CLASS**。

可以选择下面的标志中的一个：

优先级：**HIGH_PRIORITY_CLASS**

含义：指示这个进程将执行时间临界的任务，所以它必须被立即运行以保证正确。这个优先级的程序优先于正常优先级或空闲优先级的程序。

一个例子是Windows任务列表，为了保证当用户调用时可以立刻响应，放弃了对系统负荷的考虑。确保在使用高优先级时应该足够谨慎，

因为一个高优先级的CPU关联应用程序可以占用几乎全部的CPU可用时间。

优先级：**IDLE_PRIORITY_CLASS**

含义：指示这个进程的线程只有在系统空闲时才会运行并且可以被任何高优先级的任务打断。例如屏幕保护程序。空闲优先级会被子进程继承。

优先级：**NORMAL_PRIORITY_CLASS**

含义：指示这个进程没有特殊的任务调度要求。

优先级：**REALTIME_PRIORITY_CLASS**

含义：指示这个进程拥有可用的最高优先级。一个拥有实时优先级的进程的线程可以打断所有其他进程线程的执行，

包括正在执行重要任务的系统进程。例如，一个执行时间稍长一点的实时进程可能导致磁盘缓存不足或鼠标反映迟钝。





## STARTUPINFO



STARTUPINFO用于**指定新进程的主窗口特性的一个结构体**



```c
typedef struct _STARTUPINFOA {
  DWORD  cb;//值一般都是sizeof(STARTUPINFOW)
  LPSTR  lpReserved;//保留字段，目前没有使用。必须初始化为NULL
  LPSTR  lpDesktop;
  LPSTR  lpTitle;//指定子进程的窗口标题。如果lpTitle是NULL，则可执行文件的名字将用作窗口名
  DWORD  dwX;
  DWORD  dwY;
  DWORD  dwXSize;
  DWORD  dwYSize;
  DWORD  dwXCountChars;
  DWORD  dwYCountChars;
  DWORD  dwFillAttribute;//用于设定子应用程序的控制台窗口使用的文本和背景颜色
  DWORD  dwFlags;//一个标志位,用于开启某些功能,也就是让结构体成员的一些值生效
  WORD   wShowWindow;//窗口呈现的方式

  WORD   cbReserved2;//必须被初始化为0
  LPBYTE lpReserved2;//保留。必须被初始化为NULL

	//用于设定供控制台输入和输出用的缓存的句柄
  HANDLE hStdInput;//用于标识键盘缓存
  HANDLE hStdOutput;//下面2个用于标识控制台窗口的缓存
  HANDLE hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;
```



STARTUPINFO是一个结构体，用于在Windows操作系统中指定子进程的创建信息。

STARTUPINFO结构体中包含了多个成员，这些成员可以用来控制子进程的一些属性，例如性能、窗口样式等。下面是STARTUPINFO结构体中一些常用的成员变量：

`lpDesktop`: 用于指定新创建进程的窗口站和桌面。

窗口站和桌面是 Windows 操作系统中用于管理用户界面的概念。

一个窗口站可以包含多个桌面，每个桌面上可以运行不同的应用程序或窗口。通过设置 STARTUPINFO.lpDesktop，可以指定新进程将在哪个窗口站和桌面上创建窗口并显示界面。

常见的 STARTUPINFO.lpDesktop 的赋值方式有以下几种：

1. NULL：将新进程创建在当前活动桌面上。这是最常见的情况，新进程将共享当前用户的桌面。
2. "WinSta0\Default"：创建新进程在 "WinSta0" 窗口站中的 "Default" 桌面上。这是默认的窗口站和桌面名称，通常在用户登录时使用。
3. "winsta0\Winlogon"：创建新进程在 "WinSta0" 窗口站中的 "Winlogon" 桌面上。这是 Windows 登录界面所在的桌面。
4. 自定义窗口站和桌面名称：可以根据需要指定自定义的窗口站和桌面名称，前提是已经存在该窗口站和桌面。

通过指定适当的窗口站和桌面，可以控制新进程的显示方式和交互环境。注意，这需要具有相应权限的用户才能创建进程在其他桌面上。

`dwX`, `dwY`, `dwXSize`, `dwYSize`: 指定子进程窗口的位置和尺寸。

`dwFlags`: 指定功能是否启用等等

STARTF_USESIZE // 使用dwXSize和dwYSize成员

**STARTF_USESHOWWINDOW //使用wShowWindow成员**

STARTF_USEPOSITION //使用dwX和dwY成员

STARTF_USECOUNTCHARS //使用dwXCountChars和dwYCountChars成员

STARTF_USEFILLATTRIBUTE //使用dwFillAttribute成员

STARTF_USESTDHANDLES //使用hStdInput、hStdOutput和hStdError成员

STARTF_RUN_FULLSCREEN //强制在x86计算机上运行的控制台应用程序以全屏幕方式启动运行

`wShowWindow`: 指定子进程的窗口状态，包括SW_HIDE、SW_MINIMIZE、SW_MAXIMIZE、SW_NORMAL等。

参数值和函数ShowWindow()类似

```c
ShowWindow(
  hWnd: HWND;       {要显示的窗口的句柄}
  nCmdShow: Integer {选项, 参加下表}
): BOOL;
 
//uCmdShow 参数可选值:
SW_HIDE            = 0;  {隐藏, 并且任务栏也没有最小化图标}
SW_SHOWNORMAL      = 1;  {用最近的大小和位置显示, 激活}
SW_NORMAL          = 1;  {同 SW_SHOWNORMAL}
SW_SHOWMINIMIZED   = 2;  {最小化, 激活}
SW_SHOWMAXIMIZED   = 3;  {最大化, 激活}
SW_MAXIMIZE        = 3;  {同 SW_SHOWMAXIMIZED}
SW_SHOWNOACTIVATE  = 4;  {用最近的大小和位置显示, 不激活}
SW_SHOW            = 5;  {同 SW_SHOWNORMAL}
SW_MINIMIZE        = 6;  {最小化, 不激活}
SW_SHOWMINNOACTIVE = 7;  {同 SW_MINIMIZE}
SW_SHOWNA          = 8;  {同 SW_SHOWNOACTIVATE}
SW_RESTORE         = 9;  {同 SW_SHOWNORMAL}
SW_SHOWDEFAULT     = 10; {同 SW_SHOWNORMAL}
SW_MAX             = 10; {同 SW_SHOWNORMAL}
```





# Eg



