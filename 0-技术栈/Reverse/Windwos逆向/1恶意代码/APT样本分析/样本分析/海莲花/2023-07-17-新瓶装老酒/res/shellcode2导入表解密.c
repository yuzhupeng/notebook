#include <windows.h>
#include <stdio.h>


PIMAGE_SECTION_HEADER gSectionHeader;
PIMAGE_NT_HEADERS64 gNtheader;
DWORD64 gFilesize;
BYTE gxorValue;
BYTE* gfileBase;

//地址转换
DWORD32 VA2FA(DWORD32 RVA)
{
	DWORD64 imagebase = gNtheader->OptionalHeader.ImageBase;
	DWORD64 sizeofimage = gNtheader->OptionalHeader.SizeOfImage;
	int cnt = gNtheader->FileHeader.NumberOfSections;
	int i = 0;
	int ok1, ok2 = 0;
	if (RVA == 0)//不可能是0,但是导出表会传递进来0
		return 0;

	if (RVA >= imagebase)
	{
		RVA -= imagebase;//传递进来的是VA
	}
	//否者传递进来的是FA
	for (i = 0; i < cnt - 1; i++)
	{
		ok1 = RVA >= gSectionHeader[i].VirtualAddress;
		ok2 = RVA < gSectionHeader[i + 1].VirtualAddress;
		if (ok1 && ok2)
		{
			return (RVA - gSectionHeader[i].VirtualAddress + gSectionHeader[i].PointerToRawData);
		}
	}
	if (RVA < sizeofimage)
		return (RVA - gSectionHeader[i].VirtualAddress + gSectionHeader[i].PointerToRawData);
	return -1;
}
//地址转换
DWORD32 FA2VA(DWORD32 FA)
{
	DWORD64 imagebase = gNtheader->OptionalHeader.ImageBase;
	int cnt = gNtheader->FileHeader.NumberOfSections;
	int i = 0;
	int ok1, ok2 = 0;

	for (i = 0; i < cnt - 1; i++)
	{
		ok1 = FA >= gSectionHeader[i].PointerToRawData;
		ok2 = FA < gSectionHeader[i + 1].PointerToRawData;
		if (ok1 && ok2)
		{
			return (FA - gSectionHeader[i].PointerToRawData + gSectionHeader[i].VirtualAddress + imagebase);
		}
	}
	if (FA <= gFilesize)
		return (FA - gSectionHeader[i].PointerToRawData + gSectionHeader[i].VirtualAddress + imagebase);
	return -1;
}

//显示导入表
void showImport()
{
	//dll名字
	int i, j, k;
	DWORD32 imagebase = gNtheader->OptionalHeader.ImageBase;
	PIMAGE_IMPORT_DESCRIPTOR lpImport = VA2FA(gNtheader->OptionalHeader.DataDirectory[1].VirtualAddress) + gfileBase;
	PIMAGE_IMPORT_BY_NAME byname;
	DWORD64* IAT;
	DWORD64* INT;
	BYTE* DllName;
	BYTE* APIName;
	
	HMODULE hDll;

	PIMAGE_NT_HEADERS64 gNtheader_Dll;
	PIMAGE_EXPORT_DIRECTORY lp_Export_Dll;
	DWORD* AddressOfNames;
	WORD* AddressOfNameOrdinals;
	WORD IDbase;
	int numFunc;
	if (gNtheader&&gNtheader->OptionalHeader.DataDirectory[1].VirtualAddress == 0)
	{
		return;
	}
	for (i = 0; lpImport[i].Name; i++)
	{
		DllName = (char*)(VA2FA(lpImport[i].Name) + gfileBase);
		for (k = 0; ; k++)
		{
			DllName[k]^= gxorValue;
			if ( (DllName[k + 1] ^ gxorValue) == 0)
			{
				DllName[k+1] ^= gxorValue;
				break;
			}
		}
	 

		printf("%s\n", DllName);
		hDll = LoadLibraryA(DllName);
		if (1)
		{
			INT = VA2FA(lpImport[i].OriginalFirstThunk) + gfileBase;
			
			for (j = 0; INT[j]; j++)
			{
				if ( INT[j] && ((INT[j] >> 63) == 1))//ID导入
				{
					
					if (hDll)
					{
						gNtheader_Dll = (*(DWORD*)((BYTE*)hDll + 0x3c)) + (BYTE*)hDll;
						lp_Export_Dll = gNtheader_Dll->OptionalHeader.DataDirectory[0].VirtualAddress + (BYTE*)hDll;
						AddressOfNames = lp_Export_Dll->AddressOfNames + (BYTE*)hDll;
						AddressOfNameOrdinals = lp_Export_Dll->AddressOfNameOrdinals + (BYTE*)hDll;
						IDbase = lp_Export_Dll->Base;
						numFunc = lp_Export_Dll->NumberOfFunctions;
						for (k = 0; k < numFunc; k++)
						{
							if ((INT[j] & 0xffff) == AddressOfNameOrdinals[k])
							{
								break;
							}
						}
						if (k != numFunc)
							printf("	%s\n", AddressOfNames[k - IDbase] + (BYTE*)hDll);

					}
					//API_get = hDll+ *(unsigned int*)(hDll+ *(unsigned int*)((char*)&lp_Export->AddressOfFunctions + hDll)+ 4* (*(unsigned __int16*)new_INT- (unsigned __int64)*(unsigned int*)((char*)&lp_Export->Base + hDll)));
				}
				else//名称导入
				{
					byname = VA2FA(INT[j]) + gfileBase;
					APIName = byname->Name;
					for (k = 0; ; k++)
					{
						APIName[k] ^= gxorValue;
						if ((APIName[k + 1] ^ gxorValue) == 0)
						{
							APIName[k + 1] ^= gxorValue;
							break;
						}
					}
					printf("	%s\n", APIName);
				}
				
			}
		}
		FreeLibrary(hDll);
	}
	return;
}
void write2File(char* fpath, BYTE* data, int dsize)
{
	HANDLE hFile;
	DWORD dwBytesWritten;
 

	// 创建文件，如果文件存在则打开，如果文件不存在则创建
	hFile = CreateFileA(
		fpath,             // 文件名
		GENERIC_WRITE,          // 写入访问权限
		0,                      // 共享模式（0表示不共享）
		NULL,                   // 安全属性（使用默认值）
		CREATE_ALWAYS,          // 文件不存在时创建，如果存在则覆盖
		FILE_ATTRIBUTE_NORMAL,  // 文件属性（正常文件）
		NULL                    // 模板文件句柄（不使用模板）
	);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("文件创建或打开失败\n");
		return 1;
	}

	// 写入数据到文件
	if (!WriteFile(hFile, data, dsize, &dwBytesWritten, NULL)) {
		printf("写入文件失败\n");
		CloseHandle(hFile);
		return 1;
	}

	printf("成功写入 %d 字节到文件\n", dwBytesWritten);

	// 关闭文件句柄
	CloseHandle(hFile);
 }
BYTE* read2heap(char* fpath)
{
	// 打开文件
	HANDLE hFile = CreateFileA(
		fpath,  // 文件路径，根据实际情况进行修改
		GENERIC_READ,               // 访问模式，只读
		0,                          // 共享模式，不共享
		NULL,                       // 安全特性，使用默认值
		OPEN_EXISTING,              // 打开已存在的文件
		FILE_ATTRIBUTE_NORMAL,      // 文件属性，正常
		NULL                        // 模板文件句柄，不使用
	);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("无法打开文件\n");
		return 1;
	}

	// 获取文件大小
	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE) {
		printf("无法获取文件大小\n");
		CloseHandle(hFile);
		return 1;
	}

	// 分配内存
	BYTE* pMemory = VirtualAlloc(
		NULL,                   // 指定分配的地址，在此使用 NULL 表示由系统选择
		fileSize,               // 分配的内存大小，与文件大小一致
		MEM_COMMIT,             // 内存分配类型，表示提交内存
		PAGE_READWRITE          // 内存页面属性，读写权限
	);
	if (pMemory == NULL) {
		printf("无法分配内存\n");
		CloseHandle(hFile);
		return 1;
	}

	// 读取文件内容到内存
	DWORD bytesRead;
	if (!ReadFile(
		hFile,                  // 文件句柄
		pMemory,                // 缓冲区地址，用于接收文件内容
		fileSize,               // 要读取的字节数，与文件大小一致
		&bytesRead,             // 实际读取的字节数
		NULL                    // 用于传递异步 I/O 句柄，不使用
	)) {
		printf("读取文件失败\n");
		VirtualFree(pMemory, 0, MEM_RELEASE);
		CloseHandle(hFile);
		return 1;
	}
	CloseHandle(hFile);
	gFilesize = fileSize;


	gNtheader = (*(DWORD*)(pMemory + 0x3c)) + pMemory;
	gSectionHeader = (BYTE*)gNtheader + sizeof(IMAGE_NT_HEADERS64);
	gxorValue = gNtheader->FileHeader.NumberOfSymbols & 0xff;
	gfileBase = pMemory;
	showImport();
	return pMemory;
}
int main() {
	char  fpath[] = "E:/Code/normal/C/VisualStdio/Often/x64/Debug/apt/bad2.dll";
	char  fpath2[] = "E:/Code/normal/C/VisualStdio/Often/x64/Debug/apt/bad2.dll.fixed";
	BYTE* pMemory=read2heap(fpath);
	write2File(fpath2, pMemory, gFilesize);
	VirtualFree(pMemory, 0, MEM_RELEASE);
    return 0;
}
