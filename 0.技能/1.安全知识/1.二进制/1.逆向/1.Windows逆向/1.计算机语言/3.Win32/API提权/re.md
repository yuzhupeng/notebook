# 常见提权

在提权过程中，通常需要使用以下三个函数来完成：

1 **`OpenProcessToken`**: 这个函数用于打开进程的访问令牌（Access Token）。

访问令牌是一种用于标识和验证进程身份和权限的数据结构。**通过打开进程的访问令牌，可以获取到对该进程进行权限修改的权限。**



2 **`LookupPrivilegeValue`**: 这个函数用于查找指定特权（Privilege）的特权标识符（LUID）。特权标识符是一种**唯一的数值，用于表示系统中的不同特权**。在提权过程中，需要使用特权标识符来标识要修改的特权。



3 **`AdjustTokenPrivileges`**: 这个函数用于调整访问令牌（Access Token）中的特权。**通过提供特权标识符和特权的状态信息，可以将指定的特权授予或撤销访问令牌**。在提权过程中，需要使用该函数来修改访问令牌中的特权，以获取所需的权限。





下面是它们的配合过程：

1 首先，使用 **`OpenProcessToken`** 打开目标进程的访问令牌，以获取对其进行修改的权限。

2 然后，使用 **`LookupPrivilegeValue`** 查找指定特权的特权标识符（LUID），通常是 **`SE_DEBUG_NAME`** 特权。

3 接下来，创建一个 **`TOKEN_PRIVILEGES`** 结构，将要修改的特权标识符和特权的状态信息填充进去。

4 最后，使用 **`AdjustTokenPrivileges`** 函数将特权授权或撤销给访问令牌。函数返回成功表示特权调整成功。



这样，通过配合使用这三个函数，可以在提权过程中打开目标进程的访问令牌，查找特权标识符，

并将特权授权给访问令牌，以获取所需的权限。

我的小结:

获取令牌的Token : **`OpenProcessToken`** 

查找特权的uuid : **`LookupPrivilegeValue`**   

开一个新的权限结构体 `TOKEN_PRIVILEGES` ,然后根据已有的或者想要的给它初始化

然后把新的权限结构体注入到指定进程的令牌中 **`AdjustTokenPrivileges`**





关于访问令牌的一些介绍:

访问令牌（access token）是一个数据结构，用于在 Windows 操作系统中标识和验证身份认证。

每个进程都有一个访问令牌，该令牌用于标识该进程所属的用户或用户组，并规定了该进程可以执行的操作。

某些操作需要特殊的权限才能执行，这些权限被授予给某些用户或用户组，

当一个进程需要执行这些需要特殊权限的操作时,就需要使用访问令牌来检查其是否具有足够的权限。

因此，访问令牌在 Windows 操作系统中扮演着至关重要的角色，能够保障系统和应用程序的安全性和稳定性。



example

```c
/*
* 函数名: Get_Privilege
* 函数参数:
* 参数1: CAHR的权限名字
* 参数2: TRUE或者FALSE
* 函数功能: 提权
*/
#include<Windows.h>
#include<stdio.h>

BOOL Get_Privilege(CHAR* lpszPrivilege, DWORD bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if (!OpenProcessToken(
        GetCurrentProcess(),//获取当前进程的句柄
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, // 要修改权限+需要查询信息
        &hToken)//即函数调用成功后将获得的令牌句柄存储到这个变量中。
        )
    {
        printf("OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }

    if (!LookupPrivilegeValueA(//该函数用于检索给定特权名称的特权标识符
        NULL,//NULL表示将使用本地系统来执行特权查找操作
        lpszPrivilege, //字符串,指定要查找其特权标识符的特权名称。
        &luid//输出变量,这是一个指向LUID（特权标识符）变量的指针
    )
        )
    {
        printf("LookupPrivilegeValue error: %u\n", GetLastError());
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
    {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        tp.Privileges[0].Attributes = 0;
    }

    if (!AdjustTokenPrivileges(
        hToken, //从OpenProcessToken获取的
        FALSE,
        &tp,//从LookupPrivilegeValue获取的
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL
    )
        )
    {
        printf("AdjustTokenPrivileges error: %u\n", GetLastError());
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        printf("The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}
```