# PE编程

# 涉及的结构体

```c
IMAGE_DOS_HEADER*           lp_Dos;
IMAGE_FILE_HEADER*          lp_FileHeader;
IMAGE_OPTIONAL_HEADER64*    lp_option64;
IMAGE_OPTIONAL_HEADER32*    lp_option32;
IMAGE_IMPORT_DESCRIPTOR*    lp_ImportDescriptor;
IMAGE_SECTION_HEADER*       lp_SectionHeaders;
```

# 把VA转FA

```c
/**
 * 函数名称：va2fa
 * 函数功能：把传进来的VA转化为FA
 * 输入参数：
 *   - 参数1：VA值
 *   ..
 * 返回值：FA
 * 函数的前提:
 *       需要初始化一下全局变量:
         IMAGE_DOS_HEADER*           lp_Dos;
         IMAGE_FILE_HEADER*          lp_FileHeader;
         IMAGE_OPTIONAL_HEADER64*    lp_option64;
         IMAGE_OPTIONAL_HEADER32*    lp_option32;
         IMAGE_SECTION_HEADER*       lp_SectionHeaders;
 */
DWORD va2fa(DWORD va)
{
    int len;
    int i;
    len = lp_FileHeader->NumberOfSections;
    if (va == -1)
    {
        return -1;
    }
    if (isx86 && va > lp_option32->SizeOfImage)//x86的
    {
        return -1;
    }
    else if (va > lp_option64->SizeOfImage)//x64的
    {
        return -1;
    }

    for (i = 0; i < len - 1; i++)
    {
        if (va >= lp_SectionHeaders[i].VirtualAddress && va <= lp_SectionHeaders[i + 1].VirtualAddress)
        {
            break;
        }
    }
    return va - lp_SectionHeaders[i].VirtualAddress + lp_SectionHeaders[i].PointerToRawData;
}
```

# 读取PE文件

```c
/**
 * 函数名称：peFileMapping
 * 函数功能：其实就是读取二进制文件,然后把内容dump出来
 * 输入参数：
 *   - 参数1：TEXT(文件路径)
 * 返回值：开辟的内容,内容是文件的dump
 * 函数的前提: 无
 */
BYTE* peFileMapping(TCHAR* szFileName)
{
    HANDLE hFile;
    DWORD dwFileSize, dwBytesRead;
    BYTE* lpBuffer = NULL;

    hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Failed to open file, error code: %d\n", GetLastError());
        return NULL;
    }

    dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
    {
        wprintf(L"Failed to get file size, error code: %d\n", GetLastError());
        CloseHandle(hFile);
        return NULL;
    }

    lpBuffer = GlobalAlloc(GMEM_FIXED, dwFileSize + 1024);
    if (lpBuffer == NULL)
    {
        wprintf(L"Failed to allocate memory for file content, error code: %d\n", GetLastError());
        CloseHandle(hFile);
        return NULL;
    }

    if (!ReadFile(hFile, lpBuffer, dwFileSize, &dwBytesRead, NULL))
    {
        wprintf(L"Failed to read file content, error code: %d\n", GetLastError());
        GlobalFree(lpBuffer);
        CloseHandle(hFile);
        return NULL;
    }

    // Add NULL terminator at the end of the buffer
    lpBuffer[dwBytesRead] = '\0';
    CloseHandle(hFile);
    return lpBuffer;

    //记得GlobalFree(lpBuffer);
}
```

# 读取API函数在表中的位置

```c
/*
* 函数名字: GET_IAT
* 函数参数:
*   WCHAR* dllName dll的名字
*   CHAR* API_Name 要寻找的api名字
* 函数返回值: &IAT[xx];
* 函数功能:
*   返回某个API所在IAT的的指针
*/
VOID* GET_IAT(CHAR* dllName, CHAR* API_Name)//是寻找自己的IAT表
{
    HMODULE selfBase;
    HANDLE  hProcess;
    int x86 = 0;
    DWORD dwPID;
    struct _IMAGE_DOS_HEADER* lp_Dos;
    struct _IMAGE_NT_HEADERS64* lp_NT64;//x64版本
    struct _IMAGE_NT_HEADERS* lp_NT32;//x86版本
    struct _IMAGE_IMPORT_DESCRIPTOR* lp_Imp;

    DWORD64* lp_OriginalFirstThunk64;
    DWORD64* lp_FirstThunk64;

    DWORD* lp_OriginalFirstThunk32;
    DWORD* lp_FirstThunk32;

    int i, j;

    selfBase = GetModuleHandleA(NULL);
    lp_Dos = (struct _IMAGE_DOS_HEADER*)selfBase;

    dwPID = GetCurrentProcessId();
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
    IsWow64Process(hProcess, &x86);
    CloseHandle(hProcess);

    lp_NT32 = (struct _IMAGE_NT_HEADERS*)(lp_Dos->e_lfanew + (BYTE*)selfBase);
    lp_NT64 = (struct _IMAGE_NT_HEADERS64*)lp_NT32;
    if (x86)
    {
        lp_Imp = (struct _IMAGE_IMPORT_DESCRIPTOR*)(lp_NT32->OptionalHeader.DataDirectory[1].VirtualAddress + (BYTE*)selfBase);
        lp_FirstThunk32 = (DWORD*)(lp_Imp->FirstThunk + (BYTE*)selfBase);
        lp_OriginalFirstThunk32 = (DWORD*)(lp_Imp->OriginalFirstThunk + (BYTE*)selfBase);
    }
    else
    {
        lp_Imp = (struct _IMAGE_IMPORT_DESCRIPTOR*)(lp_NT64->OptionalHeader.DataDirectory[1].VirtualAddress + (BYTE*)selfBase);
        lp_FirstThunk64 = (DWORD64*)(lp_Imp->FirstThunk + (BYTE*)selfBase);
        lp_OriginalFirstThunk64 = (DWORD64*)(lp_Imp->OriginalFirstThunk + (BYTE*)selfBase);
    }
    i = 0;
    j = 0;

    //可我无法确定加载的dll名字是w还是a呀
    while (lp_Imp[i].Name)//是否结束
    {
        //是否相等
        //这dllname 是W版本还是A, 无法确定
        if (!memcmp(lp_Imp[i].Name + (BYTE*)selfBase, dllName, strlen(dllName)))
        {
            if (x86)
            {
                lp_OriginalFirstThunk32 = (DWORD*)(lp_Imp[i].OriginalFirstThunk + (BYTE*)selfBase);
                lp_FirstThunk32 = (DWORD*)(lp_Imp[i].FirstThunk + (BYTE*)selfBase);
                while (lp_OriginalFirstThunk32[j])
                {
                    if (!memcmp(lp_OriginalFirstThunk32[j] + (BYTE*)selfBase, API_Name, strlen(API_Name)))
                    {
                        //找到对应的API索引
                        return &lp_FirstThunk32[j];
                    }
                    j++;
                }
            }
            else
            {
                lp_OriginalFirstThunk64 = (DWORD64*)(lp_Imp[i].OriginalFirstThunk + (BYTE*)selfBase);
                lp_FirstThunk64 = (DWORD64*)(lp_Imp[i].FirstThunk + (BYTE*)selfBase);
                while (lp_OriginalFirstThunk64[j])
                {
                    if (!memcmp(lp_OriginalFirstThunk64[j] + (BYTE*)selfBase, API_Name, strlen(API_Name)))
                    {
                        //找到对应的API索引
                        return &lp_FirstThunk64[j];
                    }
                    j++;
                }
            }
            //是对应的dll

        }
        i++;
    }
    return 0;
}
```