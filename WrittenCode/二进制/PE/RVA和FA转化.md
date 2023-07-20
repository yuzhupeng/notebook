# x64





```c
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

```

