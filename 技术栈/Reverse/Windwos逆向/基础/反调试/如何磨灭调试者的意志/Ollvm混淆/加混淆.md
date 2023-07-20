# 加混淆

# 工具

# 手动

# 控制流平坦化

### 借鉴与[RoarCTF2019]polyre

在原文中出现了很多的do-while循环,但是循环只会进行1次

所以他的作用根本就不是循环,而仅仅是一个自上而下的运行

```
do
  v12 = v6;
while ( dword_603058 >= 10 && ((((_BYTE)dword_603054 - 1) * (_BYTE)dword_603054) & 1) != 0 );
```

while的条件是永远是false

当源代码中出现过得的类似的语句,就会杂乱不堪

![Untitled](%E5%8A%A0%E6%B7%B7%E6%B7%86%20072ee4c837bb4791b534e8e1a6424ecd/Untitled.png)

在一份网上的wp中,它的处理方式就是把while的调条件判断修改为jmp无条件

这样就消除了while循环

然后代码就会清晰一些

所以,依葫芦画瓢,我们可以写出一下简单的tea算法

```
#include <stdio.h>
#include <stdint.h>
#include<Windows.h>
uint32_t dword_603058, dword_603054;
//加密函数
void encrypt(uint32_t* v, uint32_t* k)
{
    uint32_t v0 = v[0], v1 = v[1], sum = 0, i;
    uint32_t delta = 0x9e3779b9;
    for (i = 0; i < 32; )
    {
        while (dword_603058 >= 10 && ((((BYTE)dword_603054 - 1) * (BYTE)dword_603054) & 1) != 0)
        {
            ;
        }
        do
        {
            sum += delta;
        } while (dword_603058 >= 10 && ((((BYTE)dword_603054 - 1) * (BYTE)dword_603054) & 1) != 0);
        v0 += ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        do
        {
            v1 += ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        } while (dword_603058 >= 10 && ((((BYTE)dword_603054 - 1) * (BYTE)dword_603054) & 1) != 0);
        do
        {
            i++;
        } while (dword_603058 >= 10 && ((((BYTE)dword_603054 - 1) * (BYTE)dword_603054) & 1) != 0);
    }
    v[0] = v0;
    do
    {
        v[1] = v1;
    } while (dword_603058 >= 10 && ((((BYTE)dword_603054 - 1) * (BYTE)dword_603054) & 1) != 0);
    return;
}
int main()
{

    char k[] = "8z04J8udyNAQCcg8";
    char m[] = "sRFg8VBV";
    do
    {
        encrypt(m, k);
    } while (dword_603058 >= 10 && ((((BYTE)dword_603054 - 1) * (BYTE)dword_603054) & 1) != 0);

    return 0;
}

```