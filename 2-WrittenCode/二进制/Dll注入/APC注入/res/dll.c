#define _CRT_SECURE_NO_WARNINGS
 
#include <Windows.h>

extern "C" __declspec(dllexport) int execute()
{
    return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(0, "DLL_PROCESS_ATTACH", "dll", 0);
        break;
    case DLL_THREAD_ATTACH:
        MessageBoxA(0, "DLL_THREAD_ATTACH", "dll", 0);
        break;
    case DLL_THREAD_DETACH:
        MessageBoxA(0, "DLL_THREAD_DETACH", "dll", 0);
        break;
    case DLL_PROCESS_DETACH:
        MessageBoxA(0, "DLL_PROCESS_DETACH", "dll", 0);
        break;
    }
    return TRUE;
}