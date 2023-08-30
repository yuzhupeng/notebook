

```
ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
```





# 大端序加密



```c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>


void indexFind(BYTE* input, BYTE* t, int ilen, int tlen)
{
    int i, j;
    for (i = 0; i < ilen; i++)
    {
        for (j = 0; j < tlen; j++)
        {
            if (input[i] == t[j])
            {
                input[i] = j;
                break;
            }
        }
        if (j == tlen)
        {
            input[i] = 0;
        }
    }
}
BYTE* b64_encode(BYTE* input, BYTE* t64, int ilen, int tlen)//变为4/3
{
    int i, j;
    BYTE* output = 0;
    output = malloc(ilen * 3);
    memset(output, 0, ilen * 3);
    
    if (output)
    {
        for (i = 0, j = 0; i < ilen; i += 3, j += 4)
        {
            output[j + 0] = t64[input[i + 0] >> 2];//1
            output[j + 1] = t64[(((input[i + 0] & 3) << 4) | (input[i + 1] >> 4))];//1,2
            output[j + 2] = t64[((input[i + 1] & 0xf) << 2) | (input[i + 2] >> 6)];//2,3
            output[j + 3] = t64[input[i + 2] & 0x3f];//3
        }
        switch (ilen % 3)
        {
        case 1:
            output[j - 2] = '=';
            output[j - 1] = '=';
            break;
        case 2:
            output[j - 1] = '=';
            break;
        }
      
        return output;
    }
    else
    {
        exit(-1);
    }

}
BYTE* b64_decode(BYTE* input, BYTE* t64, int ilen, int tlen)//变为3/4
{
    int i, j;
    BYTE* dec = malloc(ilen + 16);
    if (dec)
    {
        indexFind(input, t64, ilen, tlen);
        memset(dec, 0, ilen + 16);
        for (i = 0, j = 0; i < ilen; i += 4, j += 3)
        {
            dec[j + 0] = (input[i + 0] << 2) | (input[i + 1] >> 4);
            dec[j + 1] = (input[i + 1] << 4) | (input[i + 2] >> 2);
            dec[j + 2] = (input[i + 2] << 6) | (input[i + 3]);
        }
        return dec;
    }
    else
    {
        exit(-1);
    }
}
int main(int argc, BYTE* argv[])
{
    BYTE t[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    BYTE input[] = "redq";
    BYTE szEnc[] = "cmVkcQ==";
    BYTE* enc, * dec;
    enc = b64_encode(input, t, 4, 64);//CMvKCxGa
    printf("%s\n", enc);
    dec = b64_decode(szEnc, t, strlen(szEnc), 64);//CMvKCxGa
    printf("%s\n", dec);
   
    return 0;
}
```



# 小端序加密



```c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

BYTE* b64_encode(BYTE* input, BYTE* t64, int ilen, int tlen)//变为4/3
{
    int i, j;
    BYTE* output = 0;
    output = malloc(ilen * 3);
    memset(output, 0, ilen * 3);

    if (output)
    {
        for (i = 0, j = 0; i < ilen; i += 3, j += 4)
        {
            output[j + 0] = t64[input[i + 0] &0b00111111];//1
            output[j + 1] = t64[(((input[i + 0] & 0b11000000)) >> 6) | ((input[i + 1] & 0b00001111) << 2)];
            output[j + 2] = t64[((input[i + 1] & 0b11110000) >> 4) | ((input[i + 2] & 0b11) << 4)];
            output[j + 3] = t64[(input[i + 2] & 0b11111100) >> 2];//3
        }
        switch (ilen % 3)
        {
        case 1:
            output[j - 2] = '=';
            output[j - 1] = '=';
            break;
        case 2:
            output[j - 1] = '=';
            break;
        }

        return output;
    }
    else
    {
        exit(-1);
    }

}
```

