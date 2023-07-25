

# eg1



```c
#include <stdio.h>
#include <windows.h>

int main() {
    HKEY hKey;
    LONG result;
    WCHAR valueData[255];
    DWORD valueSize = sizeof(valueData);

    // 打开注册表项
    result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS)
    {
        printf("无法打开注册表项\n");
        return 1;
    }

    // 查询注册表项中的值
    result = RegQueryValueExW(hKey, L"CurrentBuild", NULL, NULL, (LPBYTE)valueData, &valueSize);
    if (result != ERROR_SUCCESS)
    {
        printf("无法查询注册表值\n");
        RegCloseKey(hKey); // 关闭注册表项
        return 1;
    }

    // 关闭注册表项
    RegCloseKey(hKey);

    // 输出查询到的值
    wprintf(L"CurrentBuild 值为：%s\n", valueData);

    return 0;
}

```

