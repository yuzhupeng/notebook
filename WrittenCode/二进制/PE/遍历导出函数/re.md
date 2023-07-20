

# v1





```c
#include <windows.h>
#include <stdio.h>

 
// 获取API函数地址
FARPROC GetFunctionAddress_x64(HMODULE moduleBase, const char* functionName)
{
    // 获取导出表
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)moduleBase;
    PIMAGE_NT_HEADERS64 ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    IMAGE_DATA_DIRECTORY* exportDirectory = &(ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
    IMAGE_EXPORT_DIRECTORY* exportTable = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)dosHeader + exportDirectory->VirtualAddress);
    // 获取函数名表和函数地址表
    DWORD* arr_AddressOfNames = (DWORD*)((BYTE*)dosHeader + exportTable->AddressOfNames);
    WORD*  arr_AddressOfNameOrdinal = (WORD*)((BYTE*)dosHeader + exportTable->AddressOfNameOrdinals);
    DWORD* Arr_AddressOfFunctions = (DWORD*)((BYTE*)dosHeader + exportTable->AddressOfFunctions);
    int i = 0, j = 0;
    int len_exportedFunctionName;
    // 遍历函数名表
    for ( i = 0; i < exportTable->NumberOfNames; i++)
    {
        const char* exportedFunctionName = (const char*)((BYTE*)dosHeader + arr_AddressOfNames[i]);

        // 比较函数名
        j = 0;
        while (exportedFunctionName[j])
        {
            j++;
        }
        len_exportedFunctionName = j;

        for (j = 0; j < len_exportedFunctionName && functionName[j]; j++)
        {
            if (exportedFunctionName[j] != functionName[j])
            {
                break;
            }
        }
        if (j == len_exportedFunctionName)
        {
            // 获取函数的序号
            WORD functionOrdinal = arr_AddressOfNameOrdinal[i];
            // 获取函数地址
            FARPROC functionAddress = (FARPROC)((BYTE*)dosHeader + Arr_AddressOfFunctions[functionOrdinal]);
            return functionAddress;
        }
    }

    return NULL; // 未找到指定的函数
}

int main()
{
    HMODULE moduleBase = LoadLibraryA("user32.dll");
    if (moduleBase != NULL)
    {
        FARPROC functionAddress = GetFunctionAddress_x64(moduleBase, "MessageBoxA");
        if (functionAddress != NULL)
        {
            // 打印函数地址
            printf("MessageBoxA function address: 0x%p\n", functionAddress);
        }
        else
        {
            printf("Failed to find the specified function.\n");
        }
    }
    else
    {
        printf("Failed to load the DLL.\n");
    }
    FreeLibrary(moduleBase);
    return 0;
}

```

