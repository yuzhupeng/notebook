





```c++
#include <iostream>
#include <windows.h>
#include <lm.h> // NetUserAdd 所在的头文件

#pragma comment(lib, "netapi32.lib") // 链接 netapi32 库

int main() {
    // 定义要创建的用户信息
    USER_INFO_1 userInfo;
    DWORD dwLevel = 1;
    DWORD dwError = 0;

    // 填充用户信息
    userInfo.usri1_name = L"新用户"; // 用户名
    userInfo.usri1_password = L"密码"; // 用户密码
    userInfo.usri1_priv = USER_PRIV_USER; // 用户权限，默认为普通用户
    userInfo.usri1_home_dir = NULL; // 用户主目录，如果为 NULL，则使用默认值
    userInfo.usri1_comment = L"新用户注释"; // 用户注释
    userInfo.usri1_flags = UF_SCRIPT; // 用户标志，默认为脚本执行标志
    userInfo.usri1_script_path = NULL; // 用户登录时执行的脚本路径，如果为 NULL，则使用默认值

    // 创建用户
    NET_API_STATUS nStatus = NetUserAdd(NULL, dwLevel, (LPBYTE)&userInfo, &dwError);

    if (nStatus == NERR_Success) {
        std::cout << "用户创建成功！" << std::endl;

        // 获取管理员用户组的SID
        PSID pAdminGroupSid;
        SID_IDENTIFIER_AUTHORITY sidAuth = SECURITY_NT_AUTHORITY;

        if (AllocateAndInitializeSid(&sidAuth, 2, SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminGroupSid)) {
            // 将新创建的用户添加到管理员用户组
            nStatus = NetLocalGroupAddMembers(NULL, L"Administrators", 3, (LPBYTE)&pAdminGroupSid, 1);

            if (nStatus == NERR_Success) {
                std::cout << "用户成功添加到管理员用户组！" << std::endl;
            }
            else {
                std::cerr << "添加用户到管理员用户组失败，错误代码: " << nStatus << std::endl;
            }

            // 释放SID内存
            FreeSid(pAdminGroupSid);
        }
        else {
            std::cerr << "获取管理员用户组SID失败！" << std::endl;
        }
    }
    else {
        std::cerr << "创建用户失败，错误代码: " << nStatus << std::endl;
    }

    return 0;
}

```

