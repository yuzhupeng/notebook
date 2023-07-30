#include <windows.h>
#include <stdio.h>
#include <winternl.h>

DWORD32 kernel32_XXXX;
DWORD32 GetKernel32_Addr(DWORD32 init_addr)
{
    unsigned int High_Page; // edi

    High_Page = init_addr & 0xFFFF0000;
    while (1)
    {
        if (*(DWORD*)High_Page == 0x00905A4D && *(DWORD*)(*(DWORD*)(High_Page + 60) + High_Page) == 0x00004550)//寻找PE和MZ标志
        {
            break;
        }
        High_Page -= 0x10000;
        if (High_Page < 0x70000000)
        {
            return 0;
        }
    }
    return High_Page;
}
DWORD getAPI(DWORD mImageBase, char* funcName)
{

    PIMAGE_EXPORT_DIRECTORY lpExport = 0; // [esp-24h] [ebp-2Ch]
    PIMAGE_DOS_HEADER lpDosheader = mImageBase;
    PIMAGE_NT_HEADERS32 lpNtheader = lpDosheader->e_lfanew + mImageBase;
    DWORD32* AddressofName; // ebx
    DWORD32* Addressoffunction; // ebx
    WORD* AddressOfOrdinal;
    int max = 0;
    int i; // edx
    int len_fucname; // [esp+0h] [ebp-8h]

    len_fucname = strlen(funcName) + 1;

    //注意都要加上Imagebase
    lpExport = lpNtheader->OptionalHeader.DataDirectory[0].VirtualAddress + mImageBase;
    AddressofName = lpExport->AddressOfNames + mImageBase;// 获取addressofname
    AddressOfOrdinal = lpExport->AddressOfNameOrdinals + mImageBase;
    Addressoffunction = lpExport->AddressOfFunctions + mImageBase;;
    max = lpExport->NumberOfNames;
    i = 0;
    for (i = 0; i < max; i++)
    {
        lpExport = lpExport;
        if (!memcmp(funcName, AddressofName[i] + mImageBase, len_fucname))//把AddressofName数组指向的名字都和目标函数进行比较,然后去找到
        {
            break;
        }
    }
    if (i == max)
    {
        return 0;
    }
    else
    {
        return  Addressoffunction[AddressOfOrdinal[i]] + mImageBase;//该函数对应的AddressOfOrdinal的成员,
    }
}
void __declspec(naked) __cdecl my_start()//naked裸函数，开辟和释放堆栈由我们自己写。这会是我们最后处理的时候,真正的entrypoint
{
    __asm
    {
        push eax;
        push ebx;
        mov  eax, dword ptr[esp + 8]; //把进入的时候的那个特殊地址给我们准备好的全局变量
        lea ebx, kernel32_XXXX;
        mov dword ptr[ebx], eax;
        pop ebx;
        pop eax;
        _emit 0xE9; //下面的数据是 jmp ( 目的地址 - (我自己的下一条地址))
        _emit 0x00;
        _emit 0x00;
        _emit 0x00;
        _emit 0x00;
    }
}
int main()
{
    char func1[] = "LoadLibraryA";
    char func2[] = "GetProcAddress";
    char func3[] = "MessageBoxA";
    DWORD32 hkernel32 = 0;
    DWORD32 huser32 = 0;

    DWORD32(__stdcall * lpLoadLibraryA)(DWORD32);//几个函数声明
    DWORD32(__stdcall * lpGetProcAddress)(DWORD32, DWORD32);//几个函数声明
    DWORD32(__stdcall * lpMessageBoxA)(DWORD32, DWORD32, DWORD32, DWORD32);//几个函数声明

    printf("%08X\n", my_start);//最高东西是辅助我们编程的
    hkernel32 = GetKernel32_Addr(kernel32_XXXX);
    if (hkernel32 == 0)
    {
        return 0;
    }
    lpLoadLibraryA = getAPI(hkernel32, func1);//获取kernel32默认装载好的函数地址
    lpGetProcAddress = getAPI(hkernel32, func2);//获取kernel32默认装载好的函数地址
    huser32 = lpLoadLibraryA("user32.dll");
    lpMessageBoxA = lpGetProcAddress(huser32, func3);
    lpMessageBoxA(0, "write by redqx", "QAQ", 0);
    return 0;
}