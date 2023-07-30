

# ntdll_RtlUserThreadStart



```c
__int64 __fastcall ntdll_RtlUserThreadStart(PTHREAD_START_ROUTINE a1, PVOID a2)
{
  unsigned int v3; // eax
  unsigned int v4; // eax

  if ( off_7FFFA9E7BFF0 )
    return sub_7FFFA9DB0BC0(0i64);
  v3 = sub_7FFFA9DB0BC0(a2);
  v4 = (ntdll_RtlExitUserThread)(v3);
  return (ntdll_NtTerminateProcess)(-1i64, v4);
}
```