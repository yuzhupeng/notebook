#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

IMAGE_DOS_HEADER*           lp_Dos;
IMAGE_FILE_HEADER*          lp_FileHeader;
IMAGE_OPTIONAL_HEADER64*    lp_option64;
IMAGE_OPTIONAL_HEADER32*    lp_option32;
IMAGE_IMPORT_DESCRIPTOR*    lp_ImportDescriptor;
IMAGE_SECTION_HEADER*       lp_SectionHeaders;

int isx86;
DWORD flen;


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
    memset(lpBuffer, 0, dwFileSize + 1024);
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
    flen = dwBytesRead;
    CloseHandle(hFile);
    return lpBuffer;

    //记得GlobalFree(lpBuffer);
}
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
/**
 * 函数名称：fa2fa
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
DWORD fa2va(DWORD fa)
{
    int len;
    int i;
    len = lp_FileHeader->NumberOfSections;
    if (fa == -1 || fa > flen)
    {
        return -1;
    }
    for (i = 0; i < len - 1; i++)
    {
        if (fa >= lp_SectionHeaders[i].PointerToRawData && fa <= lp_SectionHeaders[i + 1].PointerToRawData)
        {
            break;
        }
    }
    return fa - lp_SectionHeaders[i].PointerToRawData + lp_SectionHeaders[i].VirtualAddress;
}


/*
 * 函数名称：read
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
void readPEinfo(TCHAR* filename)
{

    lp_Dos = (IMAGE_DOS_HEADER*)peFileMapping(filename);
    if ((void*)lp_Dos == NULL)
    {
        wprintf(L"wrong file mapping\n");
        return;
    }
    lp_FileHeader = (IMAGE_FILE_HEADER*)(lp_Dos->e_lfanew + (DWORD64)lp_Dos + 4);
    lp_option64 = (IMAGE_OPTIONAL_HEADER64*)((DWORD64)lp_FileHeader + sizeof(IMAGE_FILE_HEADER));
    lp_option32 = lp_option64;

    if (lp_FileHeader->Machine == IMAGE_FILE_MACHINE_AMD64)
    {
        isx86 = 0;
        lp_SectionHeaders = (IMAGE_SECTION_HEADER*)((DWORD64)lp_option64 + sizeof(IMAGE_OPTIONAL_HEADER64));
        lp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(va2fa(lp_option64->DataDirectory[1].VirtualAddress) + (DWORD64)lp_Dos);//VA>FA>
    }
    else if (lp_FileHeader->Machine == IMAGE_FILE_MACHINE_I386)
    {
        isx86 = 1;
        lp_SectionHeaders = (IMAGE_SECTION_HEADER*)((DWORD64)lp_option32 + sizeof(IMAGE_OPTIONAL_HEADER32));
        lp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(va2fa(lp_option32->DataDirectory[1].VirtualAddress) + (DWORD64)lp_Dos);//VA>FA>
    }
    else
    {
        wprintf(L"not exe file\n");
        ExitProcess(-1);
        return;
    }
}

/*
 * 函数名称：couldInsert
 * 函数功能：判断在原来导入表的基础上,判断是否可以增加一个导入表
 * 输入参数：无
 * 返回值：
 *  - 1 表示可以增加,在原来基础上增加
 *  - 0 表示不可以在原来基础上增加
 */
DWORD couldInsert(DWORD* index)
{
    //空2个导入表,就可以添加一个新的导入表
    //一个导入表大小20字节

    char zeroArea[20];
    int i=0;

    ZeroMemory(zeroArea, 20);
    while (lp_ImportDescriptor[i].Name)
    {
        i++;
    }//出来的i是空白节区,但是个数是i+1
    *index = i;
    if (!memcmp(&lp_ImportDescriptor[i + 1], zeroArea, 20))//观察空白节区的下一个节区
    {

        return 1;//有多余的2个节区
    }
    else
    {
        return 0;
    }
}
/*
 * 函数名称：smallFindZero
 * 函数功能：判断一块连续的为零的内存
 * 输入参数:
 *  - 参数1: 判断的起始地址
 *  - 参数2: 判断结束的地址
 * 返回值：
 *  - 为零区域的长度
 *  - 1 表示继续遍历
 */
DWORD smallFindZero(BYTE* lp_Start, BYTE* lp_End)
{
    DWORD i;

    for (i = 0; ((DWORD64)lp_Start < (DWORD64)lp_End) && (*lp_Start == 0); i++)
    {
        lp_Start++;//这里面也可能会溢出
    }
    if (i == 0)
    {
        return 1;
    }
    return i;
}
/*
 * 函数名称：cloudMove
 * 函数功能：判断是否可以迁移整个导入表到已有的内存区域
 * 输入参数:
 *  - 参数1: 导入表计划的长度
 * 返回值：
 *  - 0 表示,无法迁移
 *  - 1 可以迁移,返回迁移的FA地址
 */
DWORD zeroPaddingData(DWORD addLen)//对导入表搬家
{
    BYTE* lp_Start, * lp_End;
    DWORD FA_Start = 0;
    DWORD cnt;
    lp_Start = (BYTE*)(lp_SectionHeaders[0].PointerToRawData + (DWORD64)lp_Dos);
    lp_End = (BYTE*)((DWORD64)lp_Start + flen - 0x400);//0x400是头部大小


    while (((DWORD64)lp_Start < (DWORD64)lp_End))
    {
        cnt = smallFindZero(lp_Start, lp_End);
        lp_Start += cnt;
        if (cnt > addLen)//找那个最空白,而不是最近的
        {
            addLen = cnt;
            FA_Start = (DWORD64)lp_Start - (DWORD64)lp_Dos - cnt;
            if (FA_Start % 8)
            {
                FA_Start = (FA_Start / 8 + 1) * 8;
            }
            wprintf(TEXT("%08X:%08X\n"), FA_Start, cnt);
            //break;
        }
    }
    //该地址应该是4或者8的倍数


    return FA_Start;

}
DWORD falien(DWORD checkLen)
{
    int Falien;
    if (isx86)
    {
        Falien = lp_option32->FileAlignment;
    }
    else
    {
        Falien = lp_option64->FileAlignment;
    }
    if (checkLen % Falien)//没对齐
    {
        checkLen = (checkLen / Falien + 1) * Falien;
    }
    return checkLen;
}
void inject_Dll()
{
    DWORD xx;
    DWORD fa_start;
    DWORD index;
    DWORD newImportSzie;
    DWORD newImportSziePadding=0;
    IMAGE_IMPORT_DESCRIPTOR* tlp_ImportDescriptor;
    xx=couldInsert(&index);
    newImportSzie = (index + 2) * 20 + 64;//64是导入表具体指向的4行
    if (xx==1) 
    {  
        
        fa_start = zeroPaddingData(16*4);

        //写入dll的名字
        memcpy(fa_start + (BYTE*)lp_Dos, "redqx.dll", 10); //第1行
        //写入函数的name结构体
        memcpy(fa_start + 16 + 2 + (BYTE*)lp_Dos, "funny", 6);//第2行
        //往INT写入数据(第2行)
        xx = fa2va(fa_start + 16);
        memcpy(fa_start + 32 + (BYTE*)lp_Dos, &xx, 4);

        lp_ImportDescriptor[index].Name = fa2va(fa_start);//指向第1行
        lp_ImportDescriptor[index].OriginalFirstThunk = fa2va(fa_start + 32);//指向第3行
        lp_ImportDescriptor[index].FirstThunk = fa2va(fa_start + 48);//指向第4行

        if (fa_start == 0)
        {
            //还是空间不足
            goto label1;
        }
    }
    else 
    {
    label1:
        
        fa_start = zeroPaddingData(newImportSzie);
        
        if (fa_start)
        {
            //可以迁移
            tlp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(fa_start + (BYTE*)lp_Dos);
            memcpy(fa_start+(BYTE*)lp_Dos, lp_ImportDescriptor, index * 20);            
        }
        else
        {
            fa_start = flen;
            tlp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(fa_start + (BYTE*)lp_Dos);
            newImportSziePadding = falien(newImportSzie);
            //尾部增加
            memcpy(fa_start + (BYTE*)lp_Dos, lp_ImportDescriptor, index * 20);
            if (isx86)
            {
                lp_option32->SizeOfImage += newImportSziePadding;
                //节区扩大
            }
            else
            {
                lp_option64->SizeOfImage += newImportSziePadding;
            }
            lp_SectionHeaders[lp_FileHeader->NumberOfSections - 1].SizeOfRawData += newImportSziePadding;
            lp_SectionHeaders[lp_FileHeader->NumberOfSections - 1].Misc.VirtualSize+= newImportSziePadding;
            flen += newImportSziePadding;
        }
        lp_option64->DataDirectory[1].VirtualAddress = fa2va(fa_start);
        lp_option64->DataDirectory[1].Size += 20;
        fa_start = fa_start + (index+2) * 20;


        //写入dll的名字
        memcpy(fa_start + (BYTE*)lp_Dos, "redqx.dll", 10); //第1行
        //写入函数的name结构体
        memcpy(fa_start + 16 + 2 + (BYTE*)lp_Dos, "funny", 6);//第2行
        
       
        xx = fa2va(fa_start + 16);
        //往INT写入数据(第2行)
        memcpy(fa_start + 32 + (BYTE*)lp_Dos, &xx, 4);
        //往IAT写入数据(第2行)
        memcpy(fa_start + 48 + (BYTE*)lp_Dos, &xx, 4);


        tlp_ImportDescriptor[index].Name = fa2va(fa_start);//指向第1行
        tlp_ImportDescriptor[index].OriginalFirstThunk = fa2va(fa_start + 32);//指向第3行
        tlp_ImportDescriptor[index].FirstThunk = fa2va(fa_start + 48);//指向第4行
    }
    return;
}
void writeIntoFIle(TCHAR* filename)
{
    HANDLE hFile = CreateFile(
        filename,   // 文件名
        GENERIC_WRITE,     // 访问权限为写入
        0,                 // 共享模式，这里设为0表示无共享
        NULL,              // 安全属性，这里设为NULL
        CREATE_ALWAYS,     // 创建文件方式，如果已存在则覆盖
        FILE_ATTRIBUTE_NORMAL, // 文件属性，这里设为普通文件
        NULL               // 模板文件句柄，这里设为NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        // 处理失败情况
    }
   

    BOOL bResult = WriteFile(
        hFile,               // 文件句柄
        lp_Dos,            // 要写入的数据缓冲区
        flen,              // 要写入的字节数
        NULL,                // 实际写入的字节数（可选）
        NULL                 // 异步操作（可选）
    );
    if (!bResult) {
        // 处理失败情况
    }
    CloseHandle(hFile);
    return;
}
int main()
{
    TCHAR fpath[] = TEXT("F:/notepad.exe");
    TCHAR fpath_patch[] = TEXT("F:/notepad64_patch.exe");
    readPEinfo(fpath);
    inject_Dll(fpath);
    writeIntoFIle(fpath_patch);
    GlobalFree(lp_Dos);
    return 0;
}