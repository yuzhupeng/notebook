# NtQuerySystemInformation

一个不对外开方的API

位于ntdll里面

函数定义

```c
typedef long (WINAPI* type_NtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );
```

# _SYSTEM_PROCESS_INFORMATION

该结构体是不对外开方的

所以相关结构体的定义都是从网上找的,官网没看见

具体定义见github

[mycode/Re/API_HOOK/notepad_进程隐藏/非全局钩子/NtQuerySystemInformation at 0f5b6fc83c5b4e118f8589e474fcc3df3f27a26b · redqx/mycode](https://github.com/redqx/mycode/tree/0f5b6fc83c5b4e118f8589e474fcc3df3f27a26b/Re/API_HOOK/notepad_进程隐藏/非全局钩子/NtQuerySystemInformation)