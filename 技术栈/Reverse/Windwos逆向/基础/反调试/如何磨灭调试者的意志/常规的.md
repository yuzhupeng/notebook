# 常规的

# 垃圾代码

向程序添加大量无意义的代码来增加代码调试的难度，

这就是“垃圾代码”反调试技术。

尤其是，这些垃圾代码中还含有真正有用的代码或者应用其他反调试技术时，

调试程序会变得更加困难。

但是实际的垃圾代码往往具有精巧又复杂的形态，含有大量条件分支语句和无尽的函数调用，想要跳过它们并非易

# Enigma Virtual Box有感

就是对所有调用的函数做一个转接

静态的时候,所有的函数都无法直接看见,我是说直接不是间接

只有动态分析的时候,才会函数的赋值,然后进入函数

基本是如下样子

```c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

DWORD fbox[64];

void fboxInit()
{
    fbox[0] = printf;
}
int main()
{
    char flag[64] = { 0 };
    fboxInit();
    ((int (__cdecl*)(char*, ...))(fbox[0]))("Hello World input: ");
    return 0;
}

```

在没有动态分析的时候,你是不知道函数fbox[0]是干嘛的

虽然你可以直接去 fboxInit()查看

但是,,,如果我把fboxinit()做得更加隐蔽呢???

如果我把fbox[]的函数做得更多呢??

比如下面是一个我对简单SM4算法的一个修改

```c
#define _CRT_SECURE_NO_WARNINGS
#include"head.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

DWORD lbox[64];
DWORD fbox[64];

DWORD fROL(DWORD data, int length)
{
    DWORD result = 0;
    result = (data << length) | (data >> (32 - length));
    return result;
}
/*循环右移*/
DWORD fROR(DWORD data, int length)
{
    DWORD result = 0;
    result = (data >> length) | (data << (32 - length));
    return result;
}
//(DWORD (*)(DWORD , int ))
//fROR
DWORD f(unsigned int tmp123)
{
    int v2;
    BYTE* lp = (BYTE*) & tmp123;
    v2 = (box[lp[2]] << 16) | box[lp[0]] | (box[lp[1]] << 8) | (box[lp[3]] << 24);
    return ((DWORD(*)(DWORD, int))(fbox[31]))(v2, 12) ^ (((DWORD(*)(DWORD, int))(fbox[31]))(v2, 8) ^ ((DWORD(*)(DWORD, int))(fbox[41]))(v2, 2)) ^ ((DWORD(*)(DWORD, int))(fbox[41]))(v2, 6);
}
DWORD SM42(int i0, int i1, int i2, unsigned int i3)
{
    return i0 ^ ((DWORD(*)(unsigned int))(fbox[51]))(i1 ^ i2 ^ i3);
}
void SM4()
{
    int input3[64] = { 0xD8BF92EF,0x9FCC401F,0xC5AF7647,0xBE040680 };
    char* lp = 0;
    int q = 4, j = 0;
    do
    {
        input3[q] = ((DWORD(*)(int, int, int, unsigned int))(fbox[9]))(input3[j], input3[j + 1], input3[j + 2], input3[j + 3]);
        ++j;
        ++q;
    } while (q <= 29);
    return;
}

void fboxInit()
{

}
class tls_
{
public:
    DWORD* lp_fbox;
    DWORD tmp;
    tls_()
    {
        tmp = 0;
        tmp ^= 64;
        lp_fbox = lbox + tmp;
        lp_fbox[11] = (DWORD)printf;
        lp_fbox[21] = (DWORD)scanf;
        lp_fbox[31] = (DWORD)fROL;
        lp_fbox[41] = (DWORD)fROR;
        lp_fbox[51] = (DWORD)f;
        lp_fbox[61] = (DWORD)SM4;
        lp_fbox[9] = (DWORD)SM42;
    }
};
tls_ tlsTest;
int main()
{
    char flag[64] = { 0 };
    ((int(__cdecl*)(char*, ...))(fbox[11]))((char*)"Hello World input: ");
    ((int(__cdecl*)(char*, ...))(fbox[21]))((char*)"%s", flag);
    ((int(__cdecl*)(char*, ...))(fbox[11]))((char*)"%s", flag);
    ((void (*)())(fbox[61]))();
    return 0;
}

```

反编译效果如下

![Untitled](%E5%B8%B8%E8%A7%84%E7%9A%84%208c62904db5f44f598216beb051444733/Untitled.png)

在静态情况下,我们无法做到点击查看

除非你去初始化函数fboxinit查看,知道每个fbox[index]对应了什么函数

你也无法做到对fbox[index[的重命名,,但是你可以在旁边做一些注释

![Untitled](%E5%B8%B8%E8%A7%84%E7%9A%84%208c62904db5f44f598216beb051444733/Untitled%201.png)

这种手法,只是说对静态分析有一点点干扰

但是,我们又不是不可以动态分析...除非有有些代码,有些藏得很深的代码在阻止你动态调试



# 导入表加密

效果如下

![image-20230721011326435](img/image-20230721011326435.png)

手动解密之后

![image-20230721011245218](img/image-20230721011245218.png)



具体操作,不多说

