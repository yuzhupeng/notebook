# dll调试的一些发现

之前就对如何对dll下断点了解了一下,并知道通过一些标志去下断点

[给模块下断点](../Windwos%E9%80%86%E5%90%91%2064985b5483fe46f1b2ac7b9b9dccdf2d/%E5%85%B6%E5%AE%83%20780c39c216964910a91a3d6d00537731/windwos%20%E8%B0%83%E8%AF%95%207e605d12adc94217ac3195d9c89c5b4f/%E7%BB%99%E6%A8%A1%E5%9D%97%E4%B8%8B%E6%96%AD%E7%82%B9%202577cc91fd3b4b2a82622392688b452e.md)

后来又去发现了一些东西哦

MSDN里关于禁止在DllMain里调用LoadLibrary和FreeLibrary的解释过于含糊不清

# ntdll

## LdrxCallInitRoutine

在dll_main返回的时候,会进入下面这个函数

![Untitled](dll%E8%B0%83%E8%AF%95%E7%9A%84%E4%B8%80%E4%BA%9B%E5%8F%91%E7%8E%B0%20b1f2b828ed924493a6a3e2bfd586a02d/Untitled.png)

前人通过大量的逆向工程告诉我们这些后人这个函数的参数可能如下：

```c
LdrpCallInitRoutine(Ldr->EntryPoint, Ldr>DllBase, DLL_THREAD_ATTACH, NULL); //call dll oep
```

然后我们看看这个函数的返回堆栈

![Untitled](dll%E8%B0%83%E8%AF%95%E7%9A%84%E4%B8%80%E4%BA%9B%E5%8F%91%E7%8E%B0%20b1f2b828ed924493a6a3e2bfd586a02d/Untitled%201.png)

其中就包含了dll_main的3个参数

```c
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
```

## LdrpCallInitRoutine

LdrpCallInitRoutine是一个函数，它在Windows操作系统的加载器（Loader）中使用。

它主要用于调用动态链接库（DLL）中的初始化例程（init routine）。

在Windows系统中,当一个DLL被加载到一个应用程序的地址空间中时，

它可能需要执行一些初始化操作，例如分配内存、初始化全局变量、建立钩子等。这些初始化操作由DLL内部的初始化例程负责。

LdrpCallInitRoutine函数的作用是根据加载器的要求，调用DLL中的初始化例程。它会遍历DLL的导出表，找到初始化例程的地址，

并且按照一定的调用约定（calling convention）调用这些例程。通常情况下，LdrpCallInitRoutine会在加载DLL时被调用，以确保DLL中的初始化例程得以执行。

```c
char __fastcall LdrpCallInitRoutine(int (__stdcall *a1)(int, int, int), int a2, int a3, int a4)
{
  char *v6; // eax
  struct _PEB *v7; // eax
  char *v8; // eax
  char v10; // [esp+1Bh] [ebp-19h]

  if ( ntdll_RtlGetCurrentServiceSessionId() )
    v6 = (char *)NtCurrentPeb()->HotpatchInformation + 554;
  else
    v6 = &byte_7FFE0384;
  if ( *v6 && (NtCurrentPeb()->TracingFlags & 4) != 0 )
  {
    v8 = ntdll_RtlGetCurrentServiceSessionId() ? (char *)NtCurrentPeb()->HotpatchInformation + 555 : &byte_7FFE0385;
    if ( (*v8 & 0x20) != 0 )
      LdrpLogEtwEvent(0, a3, 0, 0);
  }
  j_ntdll_RtlDebugPrintTimes(a1);
  v10 = LdrxCallInitRoutine(a1, a2, a3, a4);
  if ( ntdll_RtlGetCurrentServiceSessionId() )
    v7 = (struct _PEB *)*((unsigned __int8 *)NtCurrentPeb()->HotpatchInformation + 554);
  else
    v7 = 0;
  if ( v7 )
  {
    v7 = NtCurrentPeb();
    if ( (v7->TracingFlags & 4) != 0 )
    {
      LOBYTE(v7) = ntdll_RtlGetCurrentServiceSessionId() ? *((_BYTE *)NtCurrentPeb()->HotpatchInformation + 555) : 0;
      if ( ((unsigned __int8)v7 & 0x20) != 0 )
        LOBYTE(v7) = LdrpLogEtwEvent(-1, -1, 0, 0);
    }
  }
  if ( !v10 && a3 == 1 )
  {
    LdrpLogError(1, 0);
    LOBYTE(v7) = 0;
  }
  return (char)v7;
}
```

## LdrpInitializeThread

```c
int __thiscall LdrpInitializeThread(_DWORD *this)
{
  struct _TEB *v1; // esi
  int result; // eax
  int Tls; // eax
  int v4; // edi
  int (__stdcall *v5)(int, int, int); // ecx
  void **i; // ebx
  unsigned int v7; // eax
  int v8[9]; // [esp+10h] [ebp-7Ch] BYREF
  int v9[9]; // [esp+34h] [ebp-58h] BYREF
  int v10[4]; // [esp+58h] [ebp-34h] BYREF
  int (__stdcall *v11)(int, int, int); // [esp+68h] [ebp-24h]
  void **v12; // [esp+6Ch] [ebp-20h]
  struct _PEB *ProcessEnvironmentBlock; // [esp+70h] [ebp-1Ch]
  CPPEH_RECORD ms_exc; // [esp+74h] [ebp-18h]

  v1 = NtCurrentTeb();
  ProcessEnvironmentBlock = v1->ProcessEnvironmentBlock;
  if ( UseCOR && (v1->MuiImpersonation & 0x4000000) != 0 )
    this[44] = __ROR4__(LdrpCorExeMainRoutine, 6) ^ 0xE5B08D5A;
  ((void (__thiscall *)(struct _TEB *))RtlpInitializeThreadActivationContextStack)(v1);
  if ( (NtCurrentTeb()->MuiImpersonation & 0x80000) == 0
    || (result = (int)NtCurrentTeb(), (*(_BYTE *)(result + 4042) & 0x20) != 0) )
  {
    result = 0x2000;
    if ( (v1->MuiImpersonation & 0x20000000) == 0 )
    {
      while ( 1 )
      {
        Tls = LdrpAllocateTls();
        v4 = Tls;
        if ( Tls >= 0 )
          break;
        if ( Tls != -1073741801 )
        {
          ntdll_NtTerminateProcess(-1, Tls);
          ntdll_RtlRaiseStatus(v4);
        }
        v10[0] = -3000000;
        v10[1] = -1;
        ntdll_NtDelayExecution(0, v10);
      }
      LdrpDrainWorkQueue(0);
      LdrpAcquireLoaderLock();
      ms_exc.registration.TryLevel = 0;
      for ( i = (void **)off_77665D8C; ; i = (void **)*i )
      {
        v12 = i;
        if ( i == &off_77665D8C )
          break;
        v10[3] = (int)i;
        if ( *((int *)i[20] + 8) >= 9 && ProcessEnvironmentBlock->ImageBaseAddress != i[6] )
        {
          v7 = (unsigned int)i[13];
          if ( (v7 & 0x40000) == 0 )
          {
            v5 = (int (__stdcall *)(int, int, int))i[7];
            v11 = v5;
            v10[2] = (int)v5;
            if ( v5 )
            {
              if ( (_UNKNOWN *)((unsigned int)&unk_80004 & v7) == &unk_80004 )
              {
                if ( byte_77665DA8 )
                  goto LABEL_21;
                v9[0] = 36;
                v9[1] = 1;
                memset(&v9[2], 0, 0x1Cu);
                ntdll_RtlActivateActivationContextUnsafeFast(v9, i[18]);
                ms_exc.registration.TryLevel = 1;
                if ( *((_WORD *)i + 29) )
                  LdrpCallTlsInitializers(2, i);
                LdrpCallInitRoutine(v11, (int)i[6], 2, 0);
                ms_exc.registration.TryLevel = 0;
                ntdll_RtlDeactivateActivationContextUnsafeFast(v9, 2002312205);
              }
            }
          }
        }
      }
      if ( *((_WORD *)LdrpImageEntry + 29) && !byte_77665DA8 )
      {
        v8[0] = 36;
        v8[1] = 1;
        memset(&v8[2], 0, 0x1Cu);
        ntdll_RtlActivateActivationContextUnsafeFast(v8, *((_DWORD *)LdrpImageEntry + 18));
        ms_exc.registration.TryLevel = 2;
        LdrpCallTlsInitializers(2, LdrpImageEntry);
        ms_exc.registration.TryLevel = 0;
        ntdll_RtlDeactivateActivationContextUnsafeFast(v8, 2002312297);
      }
LABEL_21:
      ms_exc.registration.TryLevel = -2;
      LdrpReleaseLoaderLock(0, v5);
      return LdrpDropLastInProgressCount();
    }
  }
  return result;
}
```

## LdrpInitialize

```c
int __fastcall _LdrpInitialize(_DWORD *a1, int a2)
{
  struct _TEB *v2; // ebx
  int v3; // esi
  int v4; // edx
  int result; // eax
  struct _PEB *ProcessEnvironmentBlock; // edi
  int v7; // ecx
  char v8; // al
  int v9; // eax
  char v10; // cl
  int v11[2]; // [esp+10h] [ebp-40h] BYREF
  struct _RTL_USER_PROCESS_PARAMETERS *ProcessParameters; // [esp+18h] [ebp-38h]
  union _PEB::$9091FB23ACFC48B9D2023E9670FB1584 *p_CrossProcessFlags; // [esp+1Ch] [ebp-34h]
  struct _TEB *v14; // [esp+20h] [ebp-30h]
  int v15; // [esp+28h] [ebp-28h]
  int v16; // [esp+2Ch] [ebp-24h]
  _DWORD *v17; // [esp+30h] [ebp-20h]
  int v18; // [esp+34h] [ebp-1Ch]
  CPPEH_RECORD ms_exc; // [esp+38h] [ebp-18h]

  v15 = a2;
  v17 = a1;
  v2 = NtCurrentTeb();
  v14 = v2;
  v3 = 0;
  v16 = 0;
LABEL_2:
  v4 = 0x2000;
  result = _InterlockedCompareExchange(&LdrpProcessInitialized, 1, 0);
  if ( result == 1 && (v2->MuiImpersonation & 0x20000000) == 0 )
    goto LABEL_34;
  while ( 1 )
  {
    ProcessEnvironmentBlock = v2->ProcessEnvironmentBlock;
    if ( !result )
      break;
    v18 = 0;
    if ( LdrpDelegatedNtdllBase )
      return result;
    if ( !ProcessEnvironmentBlock->InheritedAddressSpace )
      goto LABEL_7;
    v3 = 0;
    LdrpInitCompleteEvent = 0;
    if ( _InterlockedCompareExchange(&LdrpProcessInitialized, 1, 2) == 2 )
    {
      if ( ProcessEnvironmentBlock->InheritedAddressSpace )
      {
        result = LdrpForkProcess();
        v3 = result;
        if ( result < 0 )
        {
LABEL_7:
          if ( (v2->MuiImpersonation & 0x400000) == 0 )
          {
            if ( LdrpForkInProgress )
            {
              ntdll_RtlAcquireSRWLockShared(&LdrpForkActiveLock);
              while ( LdrpForkInProgress )
                ntdll_RtlSleepConditionVariableSRW(&LdrpForkConditionVariable, &LdrpForkActiveLock, 0, 1);
              ntdll_RtlReleaseSRWLockShared(&LdrpForkActiveLock);
            }
            result = LdrpInitializeThread(v17);
          }
          goto LABEL_10;
        }
      }
      else
      {
        v3 = v18;
      }
      result = LdrpProcessInitializationComplete();
      goto LABEL_7;
    }
LABEL_34:
    if ( !LdrpInitCompleteEvent )
      goto LABEL_41;
    v7 = ntdll_NtWaitForSingleObject(LdrpInitCompleteEvent, 0, 0);
    if ( v7 < 0 )
    {
      v8 = ShowSnaps;
      if ( (ShowSnaps & 3) != 0 )
      {
        LdrpLogDbgPrint(
          "minkernel\\ntdll\\ldrinit.c",
          1558,
          "_LdrpInitialize",
          1,
          "NtWaitForSingleObject failed with status 0x%08lx, fallback to delay loop\n",
          v7);
        v8 = ShowSnaps;
      }
      if ( (v8 & 0x40) != 0 )
        __debugbreak();
LABEL_41:
      v11[0] = -300000;
      v11[1] = -1;
      while ( LdrpProcessInitialized == 1 )
      {
        v9 = ntdll_NtDelayExecution(0, v11);
        if ( v9 < 0 )
        {
          v10 = ShowSnaps;
          if ( (ShowSnaps & 3) != 0 )
          {
            LdrpLogDbgPrint(
              "minkernel\\ntdll\\ldrinit.c",
              1575,
              "_LdrpInitialize",
              1,
              "Delaying execution failed with status 0x%08lx\n",
              v9);
            v10 = ShowSnaps;
          }
          if ( (v10 & 0x40) != 0 )
            __debugbreak();
        }
      }
      goto LABEL_2;
    }
    result = LdrpProcessInitialized;
  }
  ntdll_NtCreateEvent(&LdrpInitCompleteEvent, &unk_1F0003, 0, 0, 0);
  HIWORD(v2->MuiImpersonation) |= 0x20u;
  ProcessEnvironmentBlock->LoaderLock = (struct _RTL_CRITICAL_SECTION *)&LdrpLoaderLock;
  LdrInitState = 0;
  p_CrossProcessFlags = (union _PEB::$9091FB23ACFC48B9D2023E9670FB1584 *)&ProcessEnvironmentBlock->CrossProcessFlags;
  _interlockedbittestandset((volatile signed __int32 *)&ProcessEnvironmentBlock->CrossProcessFlags, 1u);
  v3 = LdrInitializeMrdata();
  if ( v3 >= 0 )
  {
    ms_exc.registration.TryLevel = 0;
    ProcessParameters = ProcessEnvironmentBlock->ProcessParameters;
    if ( (ProcessParameters->Flags & 0x80000000) != 0 )
    {
      LdrpIsSecureProcess = 1;
      LdrpDetourExist = 1;
    }
    result = LdrpInitializeProcess(v17, v15);
    v3 = result;
    v18 = result;
    if ( result < 0 )
    {
      result = ShowSnaps;
      if ( (ShowSnaps & 3) != 0 )
      {
        LdrpLogDbgPrint(
          "minkernel\\ntdll\\ldrinit.c",
          1674,
          "_LdrpInitialize",
          0,
          "Process initialization failed with status 0x%08lx\n",
          v3);
        result = ShowSnaps;
      }
      if ( (result & 0x10) != 0 )
        __debugbreak();
      ms_exc.registration.TryLevel = -2;
    }
    else
    {
      if ( LdrpDelegatedNtdllBase )
      {
        ms_exc.registration.TryLevel = -2;
        return result;
      }
      LdrpLogDllState(5294);
      if ( ProcessEnvironmentBlock->MinimumStackCommit )
      {
        v3 = LdrpTouchThreadStack();
        v18 = v3;
      }
      ms_exc.registration.TryLevel = -2;
      LdrInitState = 3;
      _interlockedbittestandreset((volatile signed __int32 *)p_CrossProcessFlags, 1u);
      RtlInitializeHeapGC(LdrpThreadPool);
      if ( v3 < 0 )
        goto LABEL_57;
      result = LdrpProcessInitializationComplete();
    }
  }
  else
  {
    result = ShowSnaps;
    if ( (ShowSnaps & 3) != 0 )
    {
      LdrpLogDbgPrint(
        "minkernel\\ntdll\\ldrinit.c",
        1650,
        "_LdrpInitialize",
        0,
        "LDR:MRDATA: Process initialization failed with status 0x%08lx\n",
        v3);
      result = ShowSnaps;
    }
    if ( (result & 0x10) != 0 )
      __debugbreak();
  }
LABEL_10:
  if ( v3 >= 0 )
  {
    if ( (v2->MuiImpersonation & 0x20000000) == 0 )
      return ntdll_NtTestAlert();
    return result;
  }
LABEL_57:
  LdrpInitializationFailure(v3, v4);
  result = ntdll_NtTerminateProcess(-1, v3);
  if ( !v16 )
    ntdll_RtlRaiseStatus(v3);
  return result;
}
```

## LdrInitializeThunk

```c
int __stdcall ntdll_LdrInitializeThunk(int a1, int a2)
{
  int (__stdcall *v2)(int, int); // esi
  int v3; // eax
  int v5; // [esp-8h] [ebp-Ch]
  int v6; // [esp-4h] [ebp-8h]

  sub_775A63EF(a1, a2);
  v2 = (int (__stdcall *)(int, int))dword_776668F0;
  if ( !dword_776668F0 )
  {
    v3 = ntdll_NtContinue(a1, 1);
    ntdll_RtlRaiseStatus(v3);
  }
  ntdll_RtlDebugPrintTimes(dword_776668F0, a1, dword_776668E0);
  return v2(v5, v6);
}
```

## ntdll_Wow64Transition

第二次dll_main返回,f9

![Untitled](dll%E8%B0%83%E8%AF%95%E7%9A%84%E4%B8%80%E4%BA%9B%E5%8F%91%E7%8E%B0%20b1f2b828ed924493a6a3e2bfd586a02d/Untitled%202.png)

我在ntdll_LdrInitializeThunk开头和分支左边分别下了断点

发现_dll_mian是在在左边分支停止下来,,,而不是在函数顶部

说明是从sub_775A63EF函数返回的

```c
ntdll.dll:775A63CC call    sub_775A63EF
```

# kernel32

## ntdll_RtlUserThreadStart

## sub_775A7B03

## sub_775A7B1F

## kernel32_BaseThreadInitThunk

```c
int __fastcall BaseThreadInitThunk(int a1, int (__thiscall *a2)(_DWORD, int), int a3)
{
  NTSTATUS v3; // eax
  int result; // eax

  if ( !a1 )
  {
    v3 = a2(a2, a3);
    RtlExitUserThread(v3);
  }
  if ( (RtlGetSuiteMask() & 0x10) == 0 )
    return 0;
  result = sub_6B8200EA();
  if ( result >= 0 )
    return 0;
  return result;
}
```

## RtlUserThreadStart

```c
int __usercall RtlUserThreadStart@<eax>(int (__thiscall *a1)(_DWORD, int)@<eax>, int a2@<ebx>, int a3, int a4)
{
  if ( !dword_4B3A69DC )
    sub_4B2E7B03(a1, a2);
  return dword_4B3A69DC();
}
```

```c
void __cdecl __noreturn sub_4B2E7B03(int (__thiscall *a1)(_DWORD, int), int a2)
{
  char v2[8]; // [esp+0h] [ebp-8h] BYREF

  RtlInitializeExceptionChain(v2);
  sub_4B2E7B1F(a1, a2);
  __debugbreak();
}
```

还会涉及一些异常处理

实际上运行的也是异常处理部分

![Untitled](dll%E8%B0%83%E8%AF%95%E7%9A%84%E4%B8%80%E4%BA%9B%E5%8F%91%E7%8E%B0%20b1f2b828ed924493a6a3e2bfd586a02d/Untitled%203.png)

```c
int __fastcall sub_4B2E7B1F(int (__thiscall *a1)(_DWORD, int), int a2)
{
  int v3; // eax

  if ( !dword_4B3A5A7C )
  {
    v3 = a1(a1, a2);
    RtlExitUserThread(v3);
  }
  return (dword_4B3A5A7C)(0, a1, a2);
}
```

![Untitled](dll%E8%B0%83%E8%AF%95%E7%9A%84%E4%B8%80%E4%BA%9B%E5%8F%91%E7%8E%B0%20b1f2b828ed924493a6a3e2bfd586a02d/Untitled%204.png)

## kernel32_BaseThreadInitThunk

```c
int __fastcall kernel32_BaseThreadInitThunk(int a1, int (__stdcall *a2)(int), int a3)
{
  int v4; // eax
  int result; // eax
  int v6; // [esp-8h] [ebp-8h]

  if ( !a1 )
  {
    kernel32_BaseDumpAppcompatCacheWorker(a2, a3);
    v4 = a2(v6);//这里会进入
    ntdll_RtlExitUserThread(v4);
  }
  if ( (ntdll_RtlGetSuiteMask() & 0x10) == 0 )
    return 0;
  result = BasepInitializeTermsrvFpns();
  if ( result >= 0 )
    return 0;
  return result;
}
```

# CreateThread

```c
DWORD initterm_e()
{
  HANDLE hHandle; // [esp+0h] [ebp-4h]

  hHandle = CreateThread(0, 0, StartAddress, 0, 0, 0);
  return WaitForSingleObject(hHandle, 0xFFFFFFFF);
}
```

StartAddress 就是之前下断点的地方

# 参考链接

[从DllMain下断点到LdrpCallInitRoutine_Yuri800的博客-CSDN博客](https://blog.csdn.net/lixiangminghate/article/details/70878141)

ntdll_RtlUserThreadStart