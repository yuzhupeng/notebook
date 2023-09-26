

# 写注册表

发现写注册表不需要多大的权限

只不过会被杀软动态查杀



```c
#include <Windows.h>

int main() {
	HKEY hKey;
	char startupPath[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	//计算机\HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
	char appName[] = "MyApp";
	char appPath[] = "f:/calc.exe";

	// 打开启动项注册表键
	if (RegOpenKeyEx(HKEY_CURRENT_USER, startupPath, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
		// 添加启动项
		if (RegSetValueEx(hKey, appName, 0, REG_SZ, (BYTE*)appPath,  strlen(appPath) + 1) == ERROR_SUCCESS) {
			// 启动项添加成功
			MessageBox(NULL, "启动项添加成功", "提示", MB_OK | MB_ICONINFORMATION);
		} else {
			// 启动项添加失败
			MessageBoxA(NULL, "启动项添加失败", "错误", MB_OK | MB_ICONERROR);
		}

		// 关闭注册表键
		RegCloseKey(hKey);
	}

	return 0;
}

```





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

