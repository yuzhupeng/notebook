#include <windows.h>
#include <stdio.h>

void test1(){
    HMODULE xx= GetModuleHandleA("kernel32.dll");
    printf("org: %p\n", xx);
    return;
}
void test2(){
    HMODULE x86_kernel_base;
    __asm{
        push eax;
        mov eax, dword ptr fs : [30h];// 指向PEB结构
        mov eax, dword ptr[eax + 0Ch]; //指向LDR Ptr32 _PEB_LDR_DATA
        mov eax, dword ptr[eax + 0Ch]; //指向InLoadOrderModuleList _LIST_ENTRY
        mov eax, dword ptr[eax]; //移动_LIST_ENTRY
        mov eax, dword ptr[eax]; //指向Kernel32
        mov eax, dword ptr[eax + 18h]; //指向DllBase基址
        mov x86_kernel_base, eax;
        pop eax;
    }
    printf("Find: %p\n", x86_kernel_base);
    return;
}
int main(){
    test1();
    test2();
    return 0;
}