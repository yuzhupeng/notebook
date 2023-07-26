#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

void SMC()
{
    int i = 0;
	char flag[] = { 0x74,0x40,0x57,0x43,0x6b,0x62,0x77,0x66,0x65,0x6a,0x21,0x76,0x21,0x95,0x77,0x9c,0x58,0x92,0x5d,0x93,0x65,0x8d,0 };
    for (i = 0; i < 22; i++)
    {
        if (i & 1 == 1)
        {
            flag[i] = (flag[i] - 0x20) ^ 0x10;
        }
        else
        {
            flag[i] = (flag[i] - 0x10) ^ 0x20;
        }
    }
    MessageBox(NULL, flag, "Good Boy", MB_OK);
}
int main()
{
    char* lp_start = 0x00411750;
    __asm
    {
            mov esi, lp_start
            mov ecx, 0
     label_encode:
            cmp ecx,299
            jae label_decode_over
            xor byte ptr [esi+ecx],0x90
            inc ecx
            jmp label_encode
    label_decode_over:
            call SMC
    }
    return 0;
}