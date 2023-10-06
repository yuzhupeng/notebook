



函数大体介绍

```c++
BOOL LoadMemModuleInternal(PMEM_MODULE pMemModule, LPVOID lpPeModuleBuffer, BOOL bCallEntry);

FARPROC GetMemModuleProcInternal(PMEM_MODULE pMemModule, LPCSTR lpName);

VOID FreeMemModuleInternal(PMEM_MODULE pMemModule);

FARPROC _GetProcAddress(HMODULE hModule, LPCSTR lpName);

HMODULE _GetModuleHandle(LPCWSTR lpName);

PAPIPTR_TABLE InitApiTable();

BOOL IsValidPEFormat(PMEM_MODULE pMemModule, LPVOID lpPeModuleBuffer);

BOOL MapMemModuleSections(PMEM_MODULE pMemModule, LPVOID lpPeModuleBuffer);

BOOL RelocateModuleBase(PMEM_MODULE pMemModule);

BOOL ResolveImportTable(PMEM_MODULE pMemModule);

BOOL SetMemProtectStatus(PMEM_MODULE pMemModule);

BOOL HandleTlsData(PMEM_MODULE pMemModule);

BOOL CallModuleEntry(PMEM_MODULE pMemModule, DWORD dwReason);

FARPROC GetExportedProcAddress(PMEM_MODULE pMemModule, LPCSTR lpName);

VOID UnmapMemModule(PMEM_MODULE pMemModule);

UINT32 GetCRC32(UINT32 uInit, void* pBuf, UINT32 nBufSize);

// Memory functions
int mml_strlenA(const char* psz);

int mml_strcmpA(const char* psza, const char* pszb);

int mml_stricmpW(const wchar_t* pwsza, const wchar_t* pwszb);

wchar_t* mml_strcpyW(wchar_t* pszDest, const wchar_t* pszSrc, unsigned int nMax);

void* mml_memset(void* pv, int c, unsigned int cb);

void* mml_memmove(void* pvDest, const void* pvSrc, unsigned int cb);
```

