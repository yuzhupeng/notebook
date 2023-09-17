// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <stdio.h>
#include "windows.h"

#define DEF_PROCESS_NAME		"notepad.exe"

HINSTANCE g_hInstance = NULL;
HHOOK g_hHook = NULL;
HWND g_hWnd = NULL;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hinstDLL;
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam_virtual_key, LPARAM lParam_extro_info)
{
	char szPath[MAX_PATH] = { 0, };
	char* p = NULL;

	if (nCode >= 0)
	{
		//  0 => 按下, 1 => 松开
		if (!(lParam_extro_info & 0x80000000))//释放键帽
		{
			GetModuleFileNameA(NULL, szPath, MAX_PATH);
			p = strrchr(szPath, '\\');
			if (!_stricmp(p + 1, DEF_PROCESS_NAME))
			{
				return 1;
			}
		}
	}
	return CallNextHookEx(g_hHook, nCode, wParam_virtual_key, lParam_extro_info);
}

#ifdef __cplusplus //这是一个条件预处理指令，用于检查是否定义了宏 __cplusplus 也就是检查当前是不是c++的文件 如果是没定义该宏就执行下面那个东西
extern "C" {	//C++编译器会对函数名进行修饰（name mangling），而C语言不会，
#endif
	__declspec(dllexport) void HookStart()//导出方式函数的声明调用
	{
		g_hHook = SetWindowsHookEx(
			WH_KEYBOARD, //钩子类型
			KeyboardProc, //钩子函数
			g_hInstance, //dll的ImageBase
			0 //作用于所有线程
		);
	}

	__declspec(dllexport) void HookStop()
	{
		if (g_hHook)
		{
			UnhookWindowsHookEx(g_hHook);//取消这个钩子
			g_hHook = NULL;
		}
	}
#ifdef __cplusplus
}
#endif