#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <string.h>

 
// 根据进程名称获取PID
DWORD GetProcessIdByName(const char* processName) {
	DWORD pid = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snapshot, &pe32)) {
			do {
				if (strcmp(processName, pe32.szExeFile) == 0) {
					pid = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &pe32));
		}
		CloseHandle(snapshot);
	}
	return pid;
}
// 根据PID获取所有相应的线程ID
void GetThreadIdsByProcessId(DWORD pid,DWORD* ThreadArr,DWORD* tlen) 
{
	int cnt = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snapshot != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te32;
		te32.dwSize = sizeof(THREADENTRY32);

		if (Thread32First(snapshot, &te32)) {
			do {
				if (te32.th32OwnerProcessID == pid) {
					printf("线程ID：%d\n", te32.th32ThreadID);
					ThreadArr[cnt] = te32.th32ThreadID;
					cnt++;
				}
			} while (Thread32Next(snapshot, &te32));
		}
		CloseHandle(snapshot);
	}
	*tlen = cnt;
}
// APC注入
BOOL ApcInjectDll(char* pszProcessName, char* pszDllName)
{
	BOOL bRet = FALSE;
	DWORD dwProcessId = 0;
	DWORD pThreadId[64] = {0};
	DWORD dwThreadIdLength = 0;
	HANDLE hProcess = NULL, hThread = NULL;
	PVOID pBaseAddress = NULL;
	PVOID pLoadLibraryAFunc = NULL;
	SIZE_T dwRet = 0, dwDllPathLen = 1 + strlen(pszDllName);
	DWORD i = 0;

	do
	{
		// 根据进程名称获取PID
		dwProcessId = GetProcessIdByName(pszProcessName);
		if (dwProcessId==0)
		{
			bRet = FALSE;
			break;
		}

		// 根据PID获取所有的相应线程ID
		GetThreadIdsByProcessId(dwProcessId, pThreadId, &dwThreadIdLength);
		// 打开注入进程
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if (NULL == hProcess)
		{
			printf("OpenProcess");
			bRet = FALSE;
			break;
		}

		// 在注入进程空间申请内存
		pBaseAddress = VirtualAllocEx(hProcess, NULL, dwDllPathLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (NULL == pBaseAddress)
		{
			printf("VirtualAllocEx");
			bRet = FALSE;
			break;
		}
		// 向申请的空间中写入DLL路径数据 
		WriteProcessMemory(hProcess, pBaseAddress, pszDllName, dwDllPathLen, &dwRet);
		if (dwRet != dwDllPathLen)
		{
			printf("WriteProcessMemory");
			bRet = FALSE;
			break;
		}

		// 获取 LoadLibrary 地址
		pLoadLibraryAFunc = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
		if (NULL == pLoadLibraryAFunc)
		{
			printf("GetProcessAddress");
			bRet = FALSE;
			break;
		}

		// 遍历线程, 插入APC
		for (i = 0; i < dwThreadIdLength; i++)
		{
			// 打开线程
			hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, pThreadId[i]);
			if (hThread)
			{
				// 插入APC
				QueueUserAPC((PAPCFUNC)pLoadLibraryAFunc, hThread, (ULONG_PTR)pBaseAddress);
				// 关闭线程句柄
				CloseHandle(hThread);
				hThread = NULL;
			}
		}

		bRet = TRUE;

	} while (FALSE);

	// 释放内存
	if (hProcess)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}
	return bRet;
}


int main() {

	BOOL bRet = FALSE;
	bRet = ApcInjectDll("notepad.exe", "f:\\Dll1.dll");//Dll1.dll
 
	if (bRet)
	{
		printf("APC Inject OK.\n");
	}
	else
	{
		printf("APC Inject ERROR.\n");
	}
	return 0;

}
