# 寻找kernel32基地址

# 内存分布示意图

# 如何寻找 kernel32

# 方法1: ESP

way1是从一个最高可读地址往低处遍历模块,然后遍历他的导出表情况,然后遍历是否有GetProessAddress()情况

下面这种方法,条件比较苛刻

在exe进入start的时候

栈的情况

![Untitled](%E5%AF%BB%E6%89%BEkernel32%E5%9F%BA%E5%9C%B0%E5%9D%80%20f35bae69088f4b2e82c6e787448c5a66/Untitled.png)

这是进入exe模块EP的最初始栈的情况

然后通过该栈的地址768B00C9

往上找,从768B0000往上找,找到的第一个就是kernel32.dll的模块

所以的话,我们连导出表都不用判断了

ps:有些强制类型转化没有写出来

通过方法way1升级版我们可以实现函数的导入

```c
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
```

# 方法2: SEH

# 方法3: TEB

# 方法4: PEB

# 方法5: LDR_DATA_TABLE_ENTRY

详情参考 

[细说  LDR_DATA_TABLE_ENTRY](../../../../../%E5%9F%BA%E7%A1%80%E7%9F%A5%E8%AF%86%20952822bd9818498680b0b4839d1b186b/windows%E5%86%85%E6%A0%B8%205f7a9d00e0b14bed9221e9376a77c471/TEB64%2085e6088f79ab4e628e5b5784df617869/0x60%20QWORD%20PEB%208774d051ce214cd1a9ef190c7aa74784/0x18%20ULONGLONG%20Ldr;%2055b5316905c544eb9a3b8d7dc7afa40f/%E7%BB%86%E8%AF%B4%20LDR_DATA_TABLE_ENTRY%20250b60e17b3c4af2a6466d097a2a6d2e.md)

其实就是遍历自身模块, 有个结构体就专门记录了自身所有的模块