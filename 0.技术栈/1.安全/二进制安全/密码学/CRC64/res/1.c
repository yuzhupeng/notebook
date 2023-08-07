#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
__int64 dq_key = 88777897;
void fenc()
{
    unsigned char flag[48] = { 0 };
    long long* lp;
    long long mem_dq;
    int i,j;

    memcpy(flag, "flag{6ff29390-6c20-4c56-ba70-a95758e3d1f8}", 42);

    for (i = 0; i < 6; i++)//
    {
        lp = &flag[i * 8];
        mem_dq = *lp;
        for (j = 0; j < 64; j++)
        {
            if (mem_dq >= 0)                      
            {
                mem_dq = mem_dq << 1;
            }
            else 
            {
                mem_dq = (mem_dq << 1) ^ dq_key;
            }
        }
        *lp = mem_dq;
    }
    for (i = 0; i < 6; i++)
    { 
        for (j = 0; j < 8; j++)
        {
            printf("0x%02X,", flag[i*8+j]);
        }
        printf("\n");
    }
    printf("\n");
    return;
}
void fdec()
{

    BYTE flag[48] = { 
    0x5B,0xF6,0xEF,0x68,0x8A,0x8A,0xA9,0x38,
    0x62,0x53,0x86,0xB1,0x63,0xFF,0xEA,0x03,
    0x71,0x00,0x5D,0x69,0xC7,0x54,0x67,0x0B,
    0xB4,0x40,0xBA,0x27,0xBF,0x32,0xD5,0xC7,
    0xE1,0xBF,0x09,0xD0,0xD8,0xBF,0xF8,0xC9,
    0xF8,0x4F,0x4D,0xB5,0x95,0x01,0x00,0x00,
     
    };
    __int64 p;
    int j, i;
    for (i = 0; i < 6; i++) 
    {
        p = *((__int64*)&flag[i * 8]);
        for (j = 0; j < 64; j++) 
        {
            if (p & 1) //其实这就是根据某个特征来的
            {
                p = ((unsigned __int64)p ^ dq_key) >> 1;
                p |= 0x8000000000000000;//还原那个符号位1 

            }
            else
            {
                p = (unsigned __int64)p >>1;
            }
        }
        *((__int64*)&flag[i * 8])=p;
    }
    for (i = 0; i < 48; i++)
        printf("%c", flag[i]);
    printf("\n");
    return;
}
int main()
{
    fenc();
    fdec();
}