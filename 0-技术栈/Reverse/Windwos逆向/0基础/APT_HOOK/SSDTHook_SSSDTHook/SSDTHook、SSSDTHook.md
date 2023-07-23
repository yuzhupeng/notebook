# SSDT Hook、SSSDT Hook

SSDT 既 System Service Dispath Table。在Windows NT 下， NT 的 executive（ NTOSKRNL.EXE 的一部分）提供了核心系统服务。

由于子系统不同， API 函数的函数名也不同。 例如，要用Win32API 打开一个文件，应用程序会调用 CreateFile()，

而要用 POSIXAPI，则应用程序调用 open() 函数。这两种应用程序最终都会调用 NT executive 中的NtCreateFile() 系统服务。

用户模式（ User mode）的所有调用，如 Kernel32,User32.dll,Advapi32.dll等提供的API， 最终都封装在Ntdll.dll中,然后通过Int 2E或SYSENTER进入到内核模式， 

通过服务ID,在System Service DispatcherTable中分派系统函数。例如下图：

SSDT就是一个表，这个表中有内核调用的函数地址。

从上图可见，当用户层调用FindNextFile函数时， 最终会调用内核层的 NtQueryDirectoryFile函数， 而这个函数的地址就在SSDT表中，

 如果我们事先把这个地址改成我们特定函数的地址，那么就实现了SSDT Hook