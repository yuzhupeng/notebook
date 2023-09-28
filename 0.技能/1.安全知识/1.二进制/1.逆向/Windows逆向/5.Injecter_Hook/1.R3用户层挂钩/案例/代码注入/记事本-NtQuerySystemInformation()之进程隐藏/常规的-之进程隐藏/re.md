# 记事本-NtQuerySystemInformation() 之 进程隐藏

[局部Hook x64注入器分析](%E8%AE%B0%E4%BA%8B%E6%9C%AC-NtQuerySystemInformation()%20%E4%B9%8B%20%E8%BF%9B%E7%A8%8B%E9%9A%90%E8%97%8F%2074e4aa8d196b479ebf7f93c1ea144002/%E5%B1%80%E9%83%A8Hook%20x64%E6%B3%A8%E5%85%A5%E5%99%A8%E5%88%86%E6%9E%90%202768b58d40024926a7027fa09bb0089e.md)

[全局的hook x64分析](%E8%AE%B0%E4%BA%8B%E6%9C%AC-NtQuerySystemInformation()%20%E4%B9%8B%20%E8%BF%9B%E7%A8%8B%E9%9A%90%E8%97%8F%2074e4aa8d196b479ebf7f93c1ea144002/%E5%85%A8%E5%B1%80%E7%9A%84hook%20x64%E5%88%86%E6%9E%90%2098f16dea122147549a77001cca6791dd.md)

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-NtQuerySystemInformation()%20%E4%B9%8B%20%E8%BF%9B%E7%A8%8B%E9%9A%90%E8%97%8F%2074e4aa8d196b479ebf7f93c1ea144002/Untitled.png)

不是真正的把自己变为隐形

而是蒙蔽敌人的眼睛.让他们看不到我们

其实以我们这里为例

假如xxx是一个进程查看工具

xxx进程查看的原理是调用API: NtQuerySystemInformation

由于进程是内核对象，所以（用户模式下的程序）通过相关API就能检测到它们。用户模式下检测进程的相关API通常分为如下2类（出处：MSDN）。

```c
HANDLE WINAPI CreateToolhelp32Snapshot(
    DWORD dwFlags,
    DWORD th32ProcessID
);
BOOL EnumProcesses(
    DWORD* pProcessIds,
    DWORD cb,
    DWORD* pBytesReturned
);
```

上面2个API均在其内部调用了ntdll.NtQuerySystemInformation() API。

```c
NTSTATUS WINAPI ZwQuerySystemInformation(
  _In_      SYSTEM_INFORMATION_CLASS SystemInformationClass,
  _Inout_   PVOID                    SystemInformation,
  _In_      ULONG                    SystemInformationLength,
  _Out_opt_ PULONG                   ReturnLength
);
```

借助NtQuerySystemInformation() API可以获取运行中的所有进程信息（结构体），形成一个链表（Linkedlist）。

操作该链表（从链表中删除）即可隐藏相关进程。所

以在用户模式下不需要分别钩取CreateToolhelp32Snapshot()与EnumProcess()，只需钩取NtQuerySystemInformation() API就可隐藏指定进程。

常见的进程查看有taskmgr.exe procexp64.exe 当然还有一些用户自定义的进程查看器

而他们的进程查看原理就是通过ZwQuerySystemInformation

所以无论进程查看器如何, 我们都可以hook 全局的ZwQuerySystemInformation来实现一个进程隐藏

通俗易懂来说,

NtQuerySystemInformation会去查一个进程链表

于是我们去hook xxx的API NtQuerySystemInformation 去删除那个notepad的节点

![Untitled](%E8%AE%B0%E4%BA%8B%E6%9C%AC-NtQuerySystemInformation()%20%E4%B9%8B%20%E8%BF%9B%E7%A8%8B%E9%9A%90%E8%97%8F%2074e4aa8d196b479ebf7f93c1ea144002/Untitled%201.png)

于是删除那个节点后,就无法查看notepad的进程