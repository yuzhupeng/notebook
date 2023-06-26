#include <stdio.h>
#include <wchar.h>
#include <windows.h>

//象征性的插入一个dll
#ifdef __cplusplus
extern "C" {
#endif
    __declspec(dllexport) void funny() //形式上的导出函数
    {
        return;
    }
#ifdef __cplusplus
}
#endif

// typedef
typedef BOOL(WINAPI* PFSETWINDOWTEXTW)(HWND hWnd, LPWSTR lpString);
// globals
void* lp_org_SetWindowTextW = NULL;



/*
* 函数名字: lp_new_SetWindowTextW
* 函数参数:
*   参数1: 一个控件的句柄
*   参数2: 一个字符串的指针
* 函数功能:
*   hook原始SetWindowTextW() 的内容,然后在再调用原始的API
*/

BOOL WINAPI lp_new_SetWindowTextW(HWND hWnd, WCHAR* lpString)
{
    WCHAR* pNum = L"零一二三四五六七八九";
    WCHAR temp[2] = { 0, };
    int i = 0, nLen = 0, nIndex = 0;

    nLen = wcslen(lpString);
    for (i = 0; i < nLen; i++)
    {
        // 阿拉伯数字转中文
        if (L'0' <= lpString[i] && lpString[i] <= L'9')
        {
            temp[0] = lpString[i];
            nIndex = _wtoi(temp);//宽字符转数字
            lpString[i] = pNum[nIndex];
        }
    }
    // 调用原来的user32!SetWindowTextW() API
    return ((PFSETWINDOWTEXTW)lp_org_SetWindowTextW)(hWnd, lpString);
}


// hook_IAT  这里用东西一些固定的寻址方式寻找x86的IAT
BOOL hook_IAT(char* sz_user32Dll, PROC pfnOrg, PROC pfnNew)
{
    HMODULE hMod;
    LPCSTR szDllname;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    PIMAGE_THUNK_DATA pThunk;
    DWORD dwOldProtect, dwRVA;
    BYTE* pAddr;

    hMod = GetModuleHandleA(NULL);//获取exe进程的ImageBase
    pAddr = (BYTE*)hMod;
    pAddr += *((DWORD*)&pAddr[0x3C]);
    dwRVA = *((DWORD*)&pAddr[0x80]);
    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)hMod + dwRVA);//是导入表...exe才有导入表

    for (; pImportDesc->Name; pImportDesc++)
    {
        szDllname = (LPCSTR)((DWORD)hMod + pImportDesc->Name);
        if (!_stricmp(szDllname, sz_user32Dll))
        {
            pThunk = (PIMAGE_THUNK_DATA)((DWORD)hMod +pImportDesc->FirstThunk);
            for (; pThunk->u1.Function; pThunk++)//其实没必要这么写..别人是一个联合体..
            {
                if (pThunk->u1.Function == (DWORD)pfnOrg)
                {
                    VirtualProtect((LPVOID)&pThunk->u1.Function,4,PAGE_EXECUTE_READWRITE,&dwOldProtect);
                    pThunk->u1.Function = (DWORD)pfnNew;
                    VirtualProtect((LPVOID)&pThunk->u1.Function,4,dwOldProtect,&dwOldProtect);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        //动态的去修改iat 挂钩
    case DLL_PROCESS_ATTACH:
        lp_org_SetWindowTextW = GetProcAddress(GetModuleHandleA("user32.dll"),"SetWindowTextW");
        hook_IAT("user32.dll", lp_org_SetWindowTextW, (PROC)lp_new_SetWindowTextW);
        break;

        //动态的去修改iat 脱钩
    case DLL_PROCESS_DETACH:
        hook_IAT("user32.dll", (PROC)lp_new_SetWindowTextW, lp_org_SetWindowTextW);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}