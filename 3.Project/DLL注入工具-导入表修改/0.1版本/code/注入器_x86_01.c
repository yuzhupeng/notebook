/*
编译环境
vs2019
x86 debug
多字节字符集
*/

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>

//#define my_test
#define BUFFER_SIZE 1024

IMAGE_DOS_HEADER* lp_Dos;
IMAGE_FILE_HEADER* lp_FileHeader;
IMAGE_OPTIONAL_HEADER64* lp_option64;
IMAGE_OPTIONAL_HEADER32* lp_option32;
IMAGE_IMPORT_DESCRIPTOR* lp_ImportDescriptor;
IMAGE_SECTION_HEADER* lp_SectionHeaders;

int isx86;
DWORD flen;
void log(const char* format, ...)
{
    va_list vl;
    char szLog[512] = { 0, };

    va_start(vl, format);
    sprintf(szLog, format, vl);
    va_end(vl);

    OutputDebugStringA(szLog);
}

/**
 * 函数名称：peFileMapping
 * 函数功能：其实就是读取二进制文件,然后把内容dump出来
 * 输入参数：
 *   - 参数1：char* 文件路径
 * 返回值：开辟的内容,内容是文件的dump
 * 函数的前提: 无
 */
BYTE* peFileMapping(CHAR* szFileName)
{
    HANDLE hFile;
    DWORD dwFileSize, dwBytesRead;
    BYTE* lpBuffer = NULL;

    hFile = CreateFileA(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        log("Failed to open file, error code: %d\n", GetLastError());
        return NULL;
    }

    dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
    {
        log("Failed to get file size, error code: %d\n", GetLastError());
        CloseHandle(hFile);
        return NULL;
    }

    lpBuffer = GlobalAlloc(GMEM_FIXED, dwFileSize + 1024);
    if (lpBuffer == NULL)
    {
        log("Failed to allocate memory for file content, error code: %d\n", GetLastError());
        CloseHandle(hFile);
        return NULL;
    }
    memset(lpBuffer, 0, dwFileSize + 1024);
    if (!ReadFile(hFile, lpBuffer, dwFileSize, &dwBytesRead, NULL))
    {
        log("Failed to read file content, error code: %d\n", GetLastError());
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
void setSection_Write(DWORD fa) {
    int len;
    int i;
    len = lp_FileHeader->NumberOfSections;
    for (i = 0; i < len - 1; i++)
    {
        if (fa >= lp_SectionHeaders[i].PointerToRawData && fa <= lp_SectionHeaders[i + 1].PointerToRawData)
        {
            break;
        }
    }
    //出来就是那个节区了,,一定会找到的
    lp_SectionHeaders[i].Characteristics |= 0x80000000;
    return;
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
void readPEinfo(CHAR* filename)
{
    lp_Dos = (IMAGE_DOS_HEADER*)peFileMapping(filename);
    if ((void*)lp_Dos == NULL)
    {
        log("wrong file mapping\n");
        return;
    }
    lp_FileHeader = (IMAGE_FILE_HEADER*)(lp_Dos->e_lfanew + (DWORD)lp_Dos + 4);
    lp_option64 = (IMAGE_OPTIONAL_HEADER64*)((DWORD)lp_FileHeader + sizeof(IMAGE_FILE_HEADER));
    lp_option32 = lp_option64;

    if (lp_FileHeader->Machine == IMAGE_FILE_MACHINE_AMD64)
    {
        isx86 = 0;
        lp_SectionHeaders = (IMAGE_SECTION_HEADER*)((DWORD)lp_option64 + sizeof(IMAGE_OPTIONAL_HEADER64));
        lp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(va2fa(lp_option64->DataDirectory[1].VirtualAddress) + (DWORD)lp_Dos);//VA>FA>
    }
    else if (lp_FileHeader->Machine == IMAGE_FILE_MACHINE_I386)
    {
        isx86 = 1;
        lp_SectionHeaders = (IMAGE_SECTION_HEADER*)((DWORD)lp_option32 + sizeof(IMAGE_OPTIONAL_HEADER32));
        lp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(va2fa(lp_option32->DataDirectory[1].VirtualAddress) + (DWORD)lp_Dos);//VA>FA>
    }
    else
    {
        log("not pe file\n");
        GlobalFree(lp_Dos);
        ExitProcess(-1);
        return;
    }
}

/*
 * 函数名称：couldInsert
 * 函数功能：判断在原来导入表的基础上,判断是否可以增加一个导入表(不需要寻新的内存那种)
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
    int i = 0;
    memset(zeroArea, 0, 20);//一个导入表的大小
    while (lp_ImportDescriptor[i].Name)
    {
        i++;
    }//出来的i是空白节区
    *index = i;//这个i可能是我们要插入的地方
    if (!memcmp(&lp_ImportDescriptor[i + 1], zeroArea, 20))//观察空白节区的下一个节区
    {
        return 1;//有多余的2个节区,然后其中一个用于插入
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
DWORD findNextZeroArea_lenght(BYTE* lp_Start, BYTE* lp_End)
{
    DWORD i;
    for (i = 0; (lp_Start < lp_End) && (*lp_Start == 0); i++)
    {
        lp_Start++;//这里面也可能会溢出
    }
    if (i == 0)
    {
        return 1;//继续往后遍历,i++
    }
    return i;//返回那块为零的区域的长度
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
DWORD FindZeroArea(DWORD addLen)//对导入表搬家,
{
    BYTE* lp_Start, * lp_End;
    DWORD FA_Start = 0;
    DWORD cnt;


    //去data节去找吧...正常情况下都会有一个data节区的
    lp_Start = (BYTE*)(lp_SectionHeaders[0].PointerToRawData + (DWORD)lp_Dos);
    lp_End = lp_Start + flen;


    while ((lp_Start < lp_End))
    {
        cnt = findNextZeroArea_lenght(lp_Start, lp_End);
        //lp_Start += cnt;
        if (cnt >= addLen)//找那个最大的区域,并且地址是4的倍数
        {
            //长度得满足
            //地址是16的倍数
            if ((DWORD)lp_Start % 16 == 0) {
                log("%08X:%08X\n", FA_Start, cnt);
                //我需要知道这个东西在哪一个节区...然后设置该节区是一个可以写入的属性
                return lp_Start - (DWORD)lp_Dos;
            }
            else {
                lp_Start += 1;
            }
        }
        else {
            lp_Start += cnt;
        }
    }
    return 0;
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
/*
其实现在又有一个问题,,,
那就是IAT表是可写入的,,,所以在你寻找文件的空白区域的时候
然后选择了一个不可写入的节区,,,那么就会导致api地址写入失败
所以我们还得找一个可以写入的区域?
*/

void inject_Dll(char* dllName,char* funcName)
{
    DWORD xx;
    DWORD fa_start;
    DWORD index;
    DWORD newImportSzie;
    DWORD newImportSziePadding = 0;
    IMAGE_IMPORT_DESCRIPTOR* tlp_ImportDescriptor;
    xx = couldInsert(&index);
    //如果需要新开辟迁移导入表,index表示原来的导入表个数..但是我们要新增加一个,,同时我们还需要保持新增加的后面为空白的区域
    //所以个数就是index+1+1
    newImportSzie = (index + 2) * 20 + 64;//64是导入表具体指向的4行

    if (xx == 1)
    {
        //可以直接插入,最好的情况
        fa_start = FindZeroArea(16 * 4);
        if (fa_start == 0)
        {
            //还是空间不足
            goto label1;
        }
        setSection_Write(fa_start);

        //写入dll的名字
        memcpy(fa_start + (BYTE*)lp_Dos, dllName, strlen(dllName)); //第1行
        //写入函数的name结构体
        memcpy(fa_start + 16 + 2 + (BYTE*)lp_Dos, funcName,strlen(funcName));//第2行
        //往INT写入数据(第2行)
        xx = fa2va(fa_start + 16);
        memcpy(fa_start + 32 + (BYTE*)lp_Dos, &xx, 4);

        lp_ImportDescriptor[index].Name = fa2va(fa_start);//指向第1行
        lp_ImportDescriptor[index].OriginalFirstThunk = fa2va(fa_start + 32);//指向第3行
        lp_ImportDescriptor[index].FirstThunk = fa2va(fa_start + 48);//指向第4行

    }
    else
    {
    label1:

        fa_start = FindZeroArea(newImportSzie);
        if (fa_start)//完全迁移导入表,不需要增加文件大小
        {
            //可以迁移
            tlp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(fa_start + (BYTE*)lp_Dos);
            memcpy(fa_start + (BYTE*)lp_Dos, lp_ImportDescriptor, index * 20);//把原来的复制过去
        }
        else//增加文件大小的方式添加导入表,//最后一个节区不一定是可以写入的
        {
            fa_start = flen;
            tlp_ImportDescriptor = (IMAGE_IMPORT_DESCRIPTOR*)(fa_start + (BYTE*)lp_Dos);
            newImportSziePadding = falien(newImportSzie);//新增加多少字节
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
            lp_SectionHeaders[lp_FileHeader->NumberOfSections - 1].Misc.VirtualSize += newImportSziePadding;
            //并且要设置节是可以写的权限?
            flen += newImportSziePadding;
        }

        if (isx86) {
            lp_option32->DataDirectory[1].VirtualAddress = fa2va(fa_start);
            lp_option32->DataDirectory[1].Size += 20;
        }
        else {
            lp_option64->DataDirectory[1].VirtualAddress = fa2va(fa_start);
            lp_option64->DataDirectory[1].Size += 20;
        }

        setSection_Write(fa_start);

        fa_start = fa_start + (index + 2) * 20;


        //写入dll的名字
        memcpy(fa_start + (BYTE*)lp_Dos, dllName, strlen(dllName)); //第1行
        //写入函数的name结构体
        memcpy(fa_start + 16 + 2 + (BYTE*)lp_Dos, funcName, strlen(funcName));//第2行


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
    HANDLE hFile = CreateFileA(
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

BOOL CopyFileWrapper(char* lpExistingFileName, char* lpNewFileName, BOOL bFailIfExists) {
    BOOL ret_val = CopyFileA(
        lpExistingFileName,  // 源文件的路径和文件名
        lpNewFileName,       // 目标文件的路径和文件名
        bFailIfExists        // 如果目标文件已经存在，是否报错
    );
    if (!ret_val) {
        DWORD dw_error = GetLastError();
        log("error Dec.%d\n", dw_error);
    }
    return ret_val;
}

int main(int argc,char* argv[])
{
    CHAR path_bak[256] = { 0 };
    char funcName[] = "redqx";
#ifdef  my_test
    CHAR fpath[] = "D:\\Projects\\test\\notepad_x64.exe";
    CHAR dllName[] = "Dll_64.dll";
#else
    CHAR* fpath;
    CHAR* dllName ;
    if (argc != 3) {
        printf("USEAGE: exe_path dll_name\n");
        printf("example:\n");
        printf("inject.exe D:/redqx.exe demo.dll\n");
        return 0;
    }
    fpath = argv[1];
    dllName = argv[2];

#endif

    strcpy(path_bak, fpath);
    strcat(path_bak, "x");
    CopyFileWrapper(fpath, path_bak, FALSE);
    readPEinfo(fpath);//有一个问题,,就是我们把IAT表放入一个位置以后...IAT在写入的时候,该节去要有写入的权限的...否则IAT的初始化就有点问题的
    //如何检车一个IAT是否异常....
    inject_Dll(dllName,funcName);
    writeIntoFIle(fpath);
    GlobalFree(lp_Dos);
    return 0;
}