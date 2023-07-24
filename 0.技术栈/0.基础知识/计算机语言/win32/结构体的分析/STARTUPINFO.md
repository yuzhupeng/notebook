# STARTUPINFO



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